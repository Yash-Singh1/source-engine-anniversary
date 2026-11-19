// Copyright (C) Ipion Software GmbH 1999-2000. All rights reserved.

#ifndef IVP_ANOMALY_MANAGER_INCLUDED
#define IVP_ANOMALY_MANAGER_INCLUDED

//IVP_EXPORT_PUBLIC

class IVP_Mindist;

/********************************************************************************
 *	Name:	       	IVP_Anomaly_Limits
 *	Description:	used to identify special situations occuring during
 *			the simulation
 ********************************************************************************/
class IVP_Anomaly_Limits {
    IVP_BOOL delete_this_if_env_is_deleted;
public:

    IVP_FLOAT max_velocity;
    int       max_collisions_per_psi;
    IVP_FLOAT max_angular_velocity_per_psi;
    int       max_collision_checks_per_psi;
    IVP_FLOAT min_friction_mass;
    IVP_FLOAT max_friction_mass;
    /********************************************************************************
     *	Name:	       	IVP_Anomaly_Manager
     *	Description:	used to identify and solve special situations occuring during
     *			the simulation
     ********************************************************************************/
    inline IVP_FLOAT get_max_velocity() const { return max_velocity; }; // [m/s]**2
    inline IVP_FLOAT get_max_angular_velocity_per_psi() const { return max_angular_velocity_per_psi; }; // [radians/psi]**2
    inline int get_max_collisions_per_psi(){ return max_collisions_per_psi; };  // per core
    inline int get_max_collision_checks_per_psi() { return max_collision_checks_per_psi; };

    virtual void environment_will_be_deleted(IVP_Environment *);

    IVP_Anomaly_Limits(IVP_BOOL delete_this_if_env_is_deleted = IVP_TRUE);
    virtual ~IVP_Anomaly_Limits();
};

/********************************************************************************
 *	Name:	       	IVP_Anomaly_Manager
 *	Description:	used to solve special situations occuring during
 *			the simulation
 ********************************************************************************/
class IVP_Anomaly_Manager {
    IVP_BOOL delete_this_if_env_is_deleted;
public:
    /********************************************************************************
     *	Name:	       	IVP_Anomaly_Manager
     *	Description:	used to identify and solve special situations occuring during
     *			the simulation
     ********************************************************************************/
    
    virtual void max_velocity_exceeded(IVP_Anomaly_Limits *, IVP_Core *, IVP_U_Float_Point *velocity_in_out);
    virtual void max_angular_velocity_exceeded( IVP_Anomaly_Limits *, IVP_Core *, IVP_U_Float_Point *angular_velocity_in_out);
    virtual void inter_penetration( IVP_Mindist *mindist,IVP_Real_Object *, IVP_Real_Object *, IVP_DOUBLE);
    virtual IVP_BOOL max_collisions_exceeded_check_freezing(IVP_Anomaly_Limits *, IVP_Core *); // return true if object should be temp. freezed
#ifdef BYTEMATCH_RETAIL
    virtual int max_collision_checks_exceeded( int total_checks ); // returns the number of additional checks to do this psi
    virtual IVP_BOOL max_contacts_exceeded_check_freezing( IVP_Core **core_list, int core_count ); // return true if the cores should be temp. freezed
#endif

    virtual void environment_will_be_deleted(IVP_Environment *);

	// to solve penetration we push objects with 2* force of gravity
	// there might be situation in that this is not necessary 
	//  -> derive from IVP_Anomaly_Manager and have different implementation
    virtual IVP_FLOAT get_push_speed_penetration( IVP_Real_Object *, IVP_Real_Object *);

#ifdef BYTEMATCH_RETAIL
	void solve_inter_penetration_simple( IVP_Real_Object *, IVP_Real_Object *, IVP_DOUBLE speed_change); //push mass centers in opposite directions
#else
	void solve_inter_penetration_simple( IVP_Real_Object *, IVP_Real_Object *); //push mass centers in opposite directions
#endif

    IVP_Anomaly_Manager(IVP_BOOL delete_this_if_env_is_deleted = IVP_TRUE);
    virtual ~IVP_Anomaly_Manager();
};

#endif
