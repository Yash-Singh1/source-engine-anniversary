
#include <hk_physics/physics.h>
#include <hk_physics/constraint/local_constraint_system/local_constraint_system.h>
#include <hk_physics/simunit/psi_info.h>
#include <hk_physics/constraint/breakable_constraint/breakable_constraint_bp.h>
#include <hk_physics/constraint/breakable_constraint/breakable_constraint.h>
#include <hk_physics/core/vm_query_builder/vm_query_builder.h>
#include <hk_math/densematrix_util.h>
#include <hk_math/dense_vector.h>

#if defined (IVP_VERSION_SDK) || defined (IVP_VERSION_EVAL) // IPION
#include <ivp_environment.hxx>
#endif // IPION

hk_Breakable_Constraint::hk_Breakable_Constraint
	(
		hk_Environment* env,
		const hk_Breakable_Constraint_BP* bp
	)
	:	hk_Constraint( env,
			bp->m_real_constraint->get_rigid_body(0),
			bp->m_real_constraint->get_rigid_body(1),
			HK_PRIORITY_LOCAL_CONSTRAINT)
{
	init_breakable_constraint(bp);
}


hk_Breakable_Constraint::hk_Breakable_Constraint
	( 
		hk_Local_Constraint_System* constraint_system,
		const hk_Breakable_Constraint_BP* bp
	)
	: hk_Constraint( constraint_system,
			bp->m_real_constraint->get_rigid_body(0),
			bp->m_real_constraint->get_rigid_body(1),
			HK_PRIORITY_LOCAL_CONSTRAINT,
			bp->m_real_constraint->get_vmq_storage_size())
{
	init_breakable_constraint(bp);
}

hk_Breakable_Constraint::~hk_Breakable_Constraint()
{
	// normally constraints are deleted by the constraint solver which owns it.
	// however this subconstraint is not really added to the system.
	delete m_real_constraint;
}


void hk_Breakable_Constraint::init_constraint(const void* vbp)
{
	const hk_Breakable_Constraint_BP* bp = static_cast<const hk_Breakable_Constraint_BP*>(vbp);
	init_breakable_constraint(bp);
}

void hk_Breakable_Constraint::init_breakable_constraint(const hk_Breakable_Constraint_BP* bp)
{
	m_real_constraint = bp->m_real_constraint;
	m_linear_strength = bp->m_linear_strength;
	m_angular_strength = bp->m_angular_strength;
	m_is_broken = false;
	m_bodyMassScale[0] = bp->m_bodyMassScale[0];
	m_bodyMassScale[1] = bp->m_bodyMassScale[1];
}

void hk_Breakable_Constraint::write_to_blueprint( hk_Breakable_Constraint_BP *pOutBP )
{
	pOutBP->m_real_constraint = m_real_constraint;
	pOutBP->m_linear_strength = m_linear_strength;
	pOutBP->m_angular_strength = m_angular_strength;
	pOutBP->m_bodyMassScale[0] = m_bodyMassScale[0];
	pOutBP->m_bodyMassScale[1] = m_bodyMassScale[1];
}

int hk_Breakable_Constraint::get_vmq_storage_size()
{
//	return 0; // HACK: CB
	return m_real_constraint->get_vmq_storage_size();
}


#ifdef BYTEMATCH_RETAIL
// the constraint runs with each body's mass (and inertia) scaled by its mass scale; the destructor restores them
struct BreakableConstraintHelper
{
	BreakableConstraintHelper( hk_Rigid_Body* b, hk_real massScale )
		:	m_core(b->get_rigid_body_core()),
			m_old_lin_vel(m_core->_get_linear_velocity()),
			m_old_ang_vel(m_core->_get_spin()),
			m_body(b)
	{
		m_massScale = massScale;
		if ( massScale != 1.0f && massScale > 0 )
		{
			if ( !m_core->_is_fixed() )
			{
				hk_Rigid_Body_Core *core = b->get_rigid_body_core();
				m_saved_inertia.set4( &core->_get_inertia_and_mass() );
				m_saved_inv_inertia.set4( &core->_get_inv_inertia_and_mass() );

				hk_real mass = m_core->get_mass();
				if ( mass * massScale > 10000.0f )
				{
					massScale = 10000.0f / mass;
				}
				hk_real invScale = 1.0f / massScale;
				core->_get_inertia_and_mass().mult( massScale );
				core->_get_inertia_and_mass().hesse_val *= massScale;
				core->_get_inv_inertia_and_mass().mult( invScale );
				core->_get_inv_inertia_and_mass().hesse_val *= invScale;
				return;
			}
		}
		m_massScale = 1.0f;
	}

	~BreakableConstraintHelper()
	{
		if ( m_massScale != 1.0f && m_massScale > 0 )
		{
			hk_Rigid_Body_Core *core = m_body->get_rigid_body_core();
			core->_get_inertia_and_mass().set4( &m_saved_inertia );
			core->_get_inv_inertia_and_mass().set4( &m_saved_inv_inertia );
		}
	}

	bool breaks( hk_real linear_limit, hk_real angular_limit )
	{
		// reverse engineer the impulses and check their strength
		hk_Vector3 linear = m_core->_get_linear_velocity() - m_old_lin_vel;

		if( m_core->get_mass()*linear.length_squared() < linear_limit*linear_limit )
		{
			hk_Vector3 angular = m_core->_get_spin() - m_old_ang_vel;

			hk_Vector3 spin;
			spin.set_mul3( m_core->_get_body_inertia(), angular );

			if( spin.length_squared() < angular_limit*angular_limit )
			{
				return false;
			}
		}

		return true;
	}

	void UnapplyConstraint()
	{
		// restore old values on break
		if ( !m_core->_is_fixed() )
		{
			m_core->_get_linear_velocity() = m_old_lin_vel;
			m_core->_get_spin() = m_old_ang_vel;
		}
	}

	hk_Rigid_Body_Core* m_core;
	hk_Vector3 m_old_lin_vel;
	hk_Vector3 m_old_ang_vel;
	hk_real m_massScale;
	hk_Rigid_Body *m_body;
	IVP_U_Float_Hesse m_saved_inertia;
	IVP_U_Float_Hesse m_saved_inv_inertia;
};
#else
struct BreakableConstraintHelper
{
	BreakableConstraintHelper( hk_Rigid_Body* b )
		:	m_core(b->get_rigid_body_core()),
			m_old_lin_vel(m_core->_get_linear_velocity()),
			m_old_ang_vel(m_core->_get_spin())
	{
	}

	bool breaks( hk_real linear_limit, hk_real angular_limit )
	{
		// reverse engineer the impulses and check their strength
		hk_Vector3 linear = m_core->_get_linear_velocity() - m_old_lin_vel;

		if( m_core->get_mass()*linear.length_squared() < linear_limit*linear_limit )
		{
			hk_Vector3 angular = m_core->_get_spin() - m_old_ang_vel;

			hk_Vector3 spin;
			spin.set_mul3( m_core->_get_body_inertia(), angular );

			if( spin.length_squared() < angular_limit*angular_limit )
			{
				return false;
			}
		}

		return true;
	}

	void UnapplyConstraint()
	{
		// restore old values on break
		m_core->_get_linear_velocity() = m_old_lin_vel;
		m_core->_get_spin() = m_old_ang_vel;
	}

	hk_Rigid_Body_Core* m_core;
	hk_Vector3 m_old_lin_vel;
	hk_Vector3 m_old_ang_vel;
};
#endif

#ifdef BYTEMATCH_RETAIL
#define BREAKABLE_HELPER( name, i ) BreakableConstraintHelper name( get_rigid_body(i), m_bodyMassScale[i] )
#else
#define BREAKABLE_HELPER( name, i ) BreakableConstraintHelper name( get_rigid_body(i) )
#endif


void hk_Breakable_Constraint::FireEventIfBroken()
{
	if ( m_is_broken && get_constraint_system()->is_active() )
	{
		IVP_Environment* env = reinterpret_cast<IVP_Environment*>(get_constraint_system()->get_environment());
		// The game code will do this if it wants the constraint broken
		//get_constraint_system()->deactivate();
		
		if(env)
		{
			env->fire_event_constraint_broken(this);
		}
	}
	m_is_broken = false;
}

int	hk_Breakable_Constraint::setup_and_step_constraint( hk_PSI_Info& pi, void *mem, hk_real tau_factor, hk_real damp_factor )
{
	/*
	 * TODO(crack); this is highly outdated and needs to handle linear and angular body mass constraints..
	 */
	//HK_ASSERT(0 && "Incomplete implementation");
	m_is_broken = false;
	BREAKABLE_HELPER( ss0, 0 );
	BREAKABLE_HELPER( ss1, 1 );
	
	m_real_constraint->setup_and_step_constraint(pi, mem, tau_factor, damp_factor);

	m_is_broken = ss0.breaks(m_linear_strength, m_angular_strength) || ss1.breaks(m_linear_strength, m_angular_strength);

	FireEventIfBroken();
	// constraint event handler has disabled this constraint due to being broken
	// so reset the object's values
	if ( !get_constraint_system()->is_active() )
	{
		ss0.UnapplyConstraint();
		ss1.UnapplyConstraint();
	}
	return m_real_constraint->get_vmq_storage_size();
}

void hk_Breakable_Constraint::step_constraint( hk_PSI_Info& pi, void *mem, hk_real tau_factor, hk_real damp_factor )
{
	/*
	 * TODO(crack); this is highly outdated and needs to handle linear and angular body mass constraints..
	 */
	//HK_ASSERT(0 && "Incomplete implementation");
	//if( m_is_broken == false )
	{
		BREAKABLE_HELPER( ss0, 0 );
		BREAKABLE_HELPER( ss1, 1 );

		m_real_constraint->step_constraint(pi,mem,tau_factor,damp_factor);

		m_is_broken = ss0.breaks(m_linear_strength, m_angular_strength) || ss1.breaks(m_linear_strength, m_angular_strength);
		FireEventIfBroken();
		// constraint event handler has disabled this constraint due to being broken
		// so reset the object's values
		if ( !get_constraint_system()->is_active() )
		{
			ss0.UnapplyConstraint();
			ss1.UnapplyConstraint();
		}
	}	
}

void hk_Breakable_Constraint::apply_effector_PSI( hk_PSI_Info& pi, hk_Array<hk_Entity*>* )
{
	HK_ASSERT(0 && "never reached");
}


// HAVOK DO NOT EDIT

