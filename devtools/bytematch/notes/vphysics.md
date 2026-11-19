# vphysics (with the ivp static libraries)

Retail: depot 223 `bin/vphysics.dylib`, compared per debug-map object with
`bytematch --data-check` (clang-600.0.57 `--retail-osx32` build). The ivp code
lives in the `ivp` submodule; everything fork-only stays the native default and
the retail forms are under `BYTEMATCH_RETAIL`. Lane reports:
/Volumes/Storage/sco/byte-match-survey/lane-mod-ivp.md (round 1) and
/Volumes/Storage/sco/byte-match-survey/lane-ivp2.md (round 2). Scratch tools:
/Volumes/Storage/sco/byte-match-survey/lane-ivp2-scratch/.

Totals with `--data-check` (matched / 2963 in-scope retail functions): round 2
started at 2729 (79.36% of bytes) and ends at 2855 (86.13%). Another 27
functions differ only in `__FILE__` strings (Valve's absolute build path), which
the identity flow rewrites. Status column, relative to the round-2 start:
identical = matched before and after; edited = matched only after round 2's
source changes; new = absent before, now present and matched; differs/missing =
still not matched (the note is the matcher's first reason).

Counts in this table: {'differs': 104, 'missing': 4, 'identical': 2729, 'edited': 113, 'new': 13}.

| object | function | status | note |
|---|---|---|---|
| array | `hk_Array_Base::grow_mem(int)` | identical |  |
| ball_socket_constraint | `hk_Ball_Socket_Constraint::get_constraint_type()` | identical |  |
| ball_socket_constraint | `hk_Ball_Socket_Constraint::setup_and_step_constraint(hk_PSI_Info&, void*, float, float)` | edited |  |
| ball_socket_constraint | `hk_Ball_Socket_Constraint::hk_Ball_Socket_Constraint(hk_Environment*, hk_Ball_Socket_BP const*, hk_Rigid_Body*, hk_Rigid_Body*)` | identical |  |
| ball_socket_constraint | `hk_Ball_Socket_Constraint::hk_Ball_Socket_Constraint(hk_Local_Constraint_System*, hk_Ball_Socket_BP const*, hk_Rigid_Body*, hk_Rigid_Body*)` | identical |  |
| ball_socket_constraint | `hk_Ball_Socket_Constraint::~hk_Ball_Socket_Constraint()` | identical |  |
| ball_socket_constraint | `hk_Ball_Socket_Constraint::~hk_Ball_Socket_Constraint()` | identical |  |
| ball_socket_constraint | `hk_Ball_Socket_Constraint::init_constraint(void const*)` | identical |  |
| ball_socket_constraint | `hk_Ball_Socket_Constraint::step_constraint(hk_PSI_Info&, void*, float, float)` | identical |  |
| ball_socket_constraint | `hk_Ball_Socket_Constraint::get_constraint_dof()` | identical |  |
| ball_socket_constraint | `hk_Ball_Socket_Constraint::write_to_blueprint(hk_Ball_Socket_BP*)` | identical |  |
| ball_socket_constraint | `hk_Ball_Socket_Constraint::get_vmq_storage_size()` | identical |  |
| breakable_constraint | `hk_Breakable_Constraint::step_constraint(hk_PSI_Info&, void*, float, float)` | differs | size retail 1227 vs ours 798 |
| breakable_constraint | `hk_Breakable_Constraint::setup_and_step_constraint(hk_PSI_Info&, void*, float, float)` | differs | size retail 1237 vs ours 799 |
| breakable_constraint | `BreakableConstraintHelper::BreakableConstraintHelper(hk_Rigid_Body*, float)` | missing |  |
| breakable_constraint | `hk_Breakable_Constraint::hk_Breakable_Constraint(hk_Local_Constraint_System*, hk_Breakable_Constraint_BP const*)` | identical |  |
| breakable_constraint | `hk_Breakable_Constraint::~hk_Breakable_Constraint()` | identical |  |
| breakable_constraint | `hk_Breakable_Constraint::~hk_Breakable_Constraint()` | identical |  |
| breakable_constraint | `hk_Breakable_Constraint::init_constraint(void const*)` | identical |  |
| breakable_constraint | `hk_Breakable_Constraint::get_constraint_dof()` | identical |  |
| breakable_constraint | `hk_Breakable_Constraint::write_to_blueprint(hk_Breakable_Constraint_BP*)` | identical |  |
| breakable_constraint | `hk_Breakable_Constraint::get_constraint_type()` | identical |  |
| breakable_constraint | `hk_Breakable_Constraint::get_vmq_storage_size()` | identical |  |
| byteswap | `CByteswap::SwapFieldToTargetEndian(void*, void*, typedescription_t*)` | differs | size retail 1024 vs ours 848 |
| byteswap | `CByteswap::SwapFieldsToTargetEndian(void*, void*, datamap_t*)` | identical |  |
| characterset | `CharacterSetBuild(characterset_t*, char const*)` | identical |  |
| color_conversion | `BuildGammaTable(float, float, float, int)` | identical |  |
| constraint | `hk_Constraint::get_constraint_type()` | identical |  |
| constraint | `hk_Constraint::hk_Constraint(hk_Local_Constraint_System*, hk_Rigid_Body*, hk_Rigid_Body*, IVP_CONTROLLER_PRIORITY, int)` | identical |  |
| constraint | `hk_Constraint::~hk_Constraint()` | identical |  |
| constraint | `hk_Constraint::~hk_Constraint()` | identical |  |
| constraint | `hk_Constraint::~hk_Constraint()` | identical |  |
| constraint | `hk_Constraint::get_constraint_dof()` | identical |  |
| convar | `_GLOBAL__I_a` | identical |  |
| convar | `ConVar_Register(int, IConCommandBaseAccessor*)` | identical |  |
| convar | `ConVar_Unregister()` | identical |  |
| convar | `CEmptyConVar::~CEmptyConVar()` | identical |  |
| convar | `CEmptyConVar::~CEmptyConVar()` | identical |  |
| convar | `ConCommandBase::Create(char const*, char const*, int)` | identical |  |
| convar | `CDefaultAccessor::RegisterConCommandBase(ConCommandBase*)` | identical |  |
| convar | `ConVar::InternalSetValue(char const*)` | identical |  |
| convar | `ConVar::ChangeStringValue(char const*, float)` | identical |  |
| convar | `ConVar::InternalSetIntValue(int)` | identical |  |
| convar | `ConVar::InternalSetFloatValue(float)` | identical |  |
| convar | `ConVar::Init()` | identical |  |
| convar | `ConVar::Create(char const*, char const*, int, char const*, bool, float, bool, float, void (*)(IConVar*, char const*, float))` | identical |  |
| convar | `ConVar::ConVar(char const*, char const*, int)` | identical |  |
| convar | `ConVar::~ConVar()` | identical |  |
| convar | `ConVar::~ConVar()` | identical |  |
| convar | `CEmptyConVar::GetName() const` | identical |  |
| convar | `ConCommandBase::GetDLLIdentifier() const` | identical |  |
| convar | `non-virtual thunk to CEmptyConVar::GetName() const` | identical |  |
| convar | `CEmptyConVar::SetValue(char const*)` | identical |  |
| convar | `CEmptyConVar::SetValue(float)` | identical |  |
| convar | `CEmptyConVar::SetValue(int)` | identical |  |
| convar | `ConVar::ClampValue(float&)` | identical |  |
| convar | `ConVar::AddFlags(int)` | identical |  |
| convar | `ConVar::SetValue(char const*)` | identical |  |
| convar | `ConVar::SetValue(float)` | identical |  |
| convar | `ConVar::SetValue(int)` | identical |  |
| convar | `CEmptyConVar::IsFlagSet(int) const` | identical |  |
| convar | `ConVar::GetHelpText() const` | identical |  |
| convar | `ConVar::IsRegistered() const` | identical |  |
| convar | `ConVar::GetName() const` | identical |  |
| convar | `ConVar::IsCommand() const` | identical |  |
| convar | `ConVar::IsFlagSet(int) const` | identical |  |
| convar | `non-virtual thunk to CEmptyConVar::SetValue(char const*)` | identical |  |
| convar | `non-virtual thunk to CEmptyConVar::SetValue(float)` | identical |  |
| convar | `non-virtual thunk to CEmptyConVar::SetValue(int)` | identical |  |
| convar | `non-virtual thunk to ConVar::SetValue(char const*)` | identical |  |
| convar | `non-virtual thunk to ConVar::SetValue(float)` | identical |  |
| convar | `non-virtual thunk to ConVar::SetValue(int)` | identical |  |
| convar | `non-virtual thunk to CEmptyConVar::IsFlagSet(int) const` | identical |  |
| convar | `non-virtual thunk to ConVar::GetName() const` | identical |  |
| convar | `non-virtual thunk to ConVar::IsFlagSet(int) const` | identical |  |
| convert | `ConvertMatrixToHL(IVP_U_Matrix const&, matrix3x4_t&)` | identical |  |
| convert | `ConvertMatrixToIVP(matrix3x4_t const&, IVP_U_Matrix&)` | identical |  |
| convert | `ConvertRotationToHL(IVP_U_Quat const&, QAngle&)` | identical |  |
| convert | `ConvertRotationToHL(IVP_U_Matrix3 const&, QAngle&)` | identical |  |
| convert | `TransformIVPToLocal(IVP_U_Point const&, IVP_U_Point&, IVP_Real_Object*, bool)` | edited |  |
| convert | `TransformLocalToIVP(IVP_U_Float_Point const&, IVP_U_Point&, IVP_Real_Object*, bool)` | edited |  |
| convert | `ConvertRotationToIVP(QAngle const&, IVP_U_Quat&)` | identical |  |
| convert | `ConvertRotationToIVP(QAngle const&, IVP_U_Matrix3&)` | identical |  |
| convert | `ConvertCoordinateAxisToIVP(int)` | identical |  |
| convert | `TransformLocalToIVP(IVP_U_Float_Point const&, IVP_U_Float_Point&, IVP_Real_Object*, bool)` | identical |  |
| datamanager | `CUtlMemory<UtlLinkedListElem_t<CUtlMultiList<CDataManagerBase::resource_lru_element_t, unsigned short>::List_t, unsigned short>, unsigned short>::Grow` | identical |  |
| datamanager | `CUtlMemory<CUtlMultiList<CDataManagerBase::resource_lru_element_t, unsigned short>::ListElem_t, int>::Grow(int)` | identical |  |
| datamanager | `CUtlMultiList<CDataManagerBase::resource_lru_element_t, unsigned short>::Alloc()` | identical |  |
| datamanager | `CUtlMultiList<CDataManagerBase::resource_lru_element_t, unsigned short>::~CUtlMultiList()` | identical |  |
| datamanager | `CUtlLinkedList<CUtlMultiList<CDataManagerBase::resource_lru_element_t, unsigned short>::List_t, unsigned short, false, unsigned short, CUtlMemory<UtlL` | identical |  |
| datamanager | `CDataManagerBase::CreateHandle(bool)` | identical |  |
| datamanager | `CDataManagerBase::LockResource(memhandle_t__*)` | identical |  |
| datamanager | `CDataManagerBase::UnlockResource(memhandle_t__*)` | identical |  |
| datamanager | `CDataManagerBase::GetForFreeByIndex(unsigned short)` | identical |  |
| datamanager | `CDataManagerBase::StoreResourceInHandle(unsigned short, void*, unsigned int)` | identical |  |
| datamanager | `CDataManagerBase::FlushAll()` | identical |  |
| datamanager | `CDataManagerBase::BreakLock(memhandle_t__*)` | identical |  |
| datamanager | `CDataManagerBase::CDataManagerBase(unsigned int)` | identical |  |
| datamanager | `CDataManagerBase::~CDataManagerBase()` | identical |  |
| datamanager | `CDataManagerBase::~CDataManagerBase()` | identical |  |
| datamanager | `CDataManagerBase::~CDataManagerBase()` | identical |  |
| datamanager | `CUtlMultiList<CDataManagerBase::resource_lru_element_t, unsigned short>::LinkBefore(unsigned short, unsigned short, unsigned short)` | identical |  |
| datamanager | `CUtlLinkedList<CUtlMultiList<CDataManagerBase::resource_lru_element_t, unsigned short>::List_t, unsigned short, false, unsigned short, CUtlMemory<UtlL` | identical |  |
| datamanager | `CDataManagerBase::EnsureCapacity(unsigned int)` | identical |  |
| datamanager | `CDataManagerBase::DestroyResource(memhandle_t__*)` | identical |  |
| datamanager | `CDataManagerBase::Lock()` | identical |  |
| datamanager | `CDataManagerBase::Unlock()` | identical |  |
| datamanager | `CDataManagerBase::TryLock()` | identical |  |
| densematrix_util | `hk_Dense_Matrix_Util::invert_5x5(hk_Fixed_Dense_Matrix<5>&, float)` | identical |  |
| densematrix_util | `hk_Dense_Matrix_Util::invert_6x6(hk_Fixed_Dense_Matrix<6>&, float)` | identical |  |
| densematrix_util | `hk_Dense_Matrix_Util::invert_3x3_symmetric(hk_Dense_Matrix&, float)` | identical |  |
| densematrix_util | `hk_Dense_Matrix_Util::mult(hk_Dense_Matrix const&, hk_Dense_Vector const&, hk_Dense_Vector&)` | identical |  |
| filesystem_helpers | `ParseFileInternal(char const*, char*, bool*, characterset_t*, unsigned long)` | identical |  |
| fixed_constraint | `hk_Fixed_Constraint::step_constraint(hk_PSI_Info&, void*, float, float)` | identical |  |
| fixed_constraint | `hk_Fixed_Constraint::get_constraint_type()` | identical |  |
| fixed_constraint | `hk_Fixed_Constraint::setup_and_step_constraint(hk_PSI_Info&, void*, float, float)` | identical |  |
| fixed_constraint | `hk_Fixed_Constraint::hk_Fixed_Constraint(hk_Environment*, hk_Fixed_BP const*, hk_Rigid_Body*, hk_Rigid_Body*)` | identical |  |
| fixed_constraint | `hk_Fixed_Constraint::hk_Fixed_Constraint(hk_Local_Constraint_System*, hk_Fixed_BP const*, hk_Rigid_Body*, hk_Rigid_Body*)` | identical |  |
| fixed_constraint | `hk_Fixed_Constraint::~hk_Fixed_Constraint()` | identical |  |
| fixed_constraint | `hk_Rigid_Body_Binary_EF::get_effected_entities(hk_Array<hk_Rigid_Body*>&)` | identical |  |
| fixed_constraint | `hk_Fixed_Constraint::init_constraint(void const*)` | identical |  |
| fixed_constraint | `hk_Fixed_Constraint::get_constraint_dof()` | identical |  |
| fixed_constraint | `hk_Fixed_Constraint::write_to_blueprint(hk_Fixed_BP*)` | identical |  |
| fixed_constraint | `hk_Fixed_Constraint::get_vmq_storage_size()` | identical |  |
| fixed_constraint | `hk_Fixed_Constraint::~hk_Fixed_Constraint()` | identical |  |
| hinge_bp_builder | `hk_Hinge_BP_Builder::set_axis_ws(hk_Rigid_Body*, hk_Rigid_Body*, hk_Vector3 const&)` | identical |  |
| hinge_bp_builder | `hk_Hinge_BP_Builder::set_axis_perp_os(int, hk_Vector3 const&)` | identical |  |
| hinge_bp_builder | `hk_Hinge_BP_Builder::set_angular_motor(float, float)` | edited |  |
| hinge_bp_builder | `hk_Hinge_BP_Builder::set_angular_limits(float, float)` | edited |  |
| hinge_bp_builder | `hk_Hinge_BP_Builder::set_position_os(int, hk_Vector3 const&)` | identical |  |
| hinge_bp_builder | `hk_Hinge_BP_Builder::set_tau(float)` | identical |  |
| hinge_constraint | `hk_Hinge_Constraint::step_constraint(hk_PSI_Info&, void*, float, float)` | identical |  |
| hinge_constraint | `hk_Hinge_Constraint::get_constraint_type()` | identical |  |
| hinge_constraint | `hk_Hinge_Constraint::init_hinge_constraint(hk_Hinge_BP const*)` | edited |  |
| hinge_constraint | `hk_Hinge_Constraint::setup_and_step_constraint(hk_PSI_Info&, void*, float, float)` | identical |  |
| hinge_constraint | `hk_Hinge_Constraint::set_motor(float, float)` | edited |  |
| hinge_constraint | `hk_Hinge_Constraint::hk_Hinge_Constraint(hk_Environment*, hk_Hinge_BP const*, hk_Rigid_Body*, hk_Rigid_Body*)` | identical |  |
| hinge_constraint | `hk_Hinge_Constraint::hk_Hinge_Constraint(hk_Local_Constraint_System*, hk_Hinge_BP const*, hk_Rigid_Body*, hk_Rigid_Body*)` | identical |  |
| hinge_constraint | `hk_Hinge_Constraint::~hk_Hinge_Constraint()` | identical |  |
| hinge_constraint | `hk_Constraint_Limit_Util::do_angular_limit(hk_PSI_Info&, hk_Rigid_Body*, hk_Vector3&, float, hk_Rigid_Body*, hk_Constraint_Limit&, float, float)` | identical |  |
| hinge_constraint | `hk_Hinge_Constraint::init_constraint(void const*)` | identical |  |
| hinge_constraint | `hk_Hinge_Constraint::get_constraint_dof()` | identical |  |
| hinge_constraint | `hk_Hinge_Constraint::write_to_blueprint(hk_Hinge_BP*)` | identical |  |
| hinge_constraint | `hk_Hinge_Constraint::get_vmq_storage_size()` | identical |  |
| hinge_constraint | `hk_Hinge_Constraint::~hk_Hinge_Constraint()` | identical |  |
| interface | `CreateInterface` | identical |  |
| interface | `CreateInterfaceInternal(char const*, int*)` | identical |  |
| interface | `InterfaceReg::InterfaceReg(void* (*)(), char const*)` | identical |  |
| interface | `Sys_GetFactoryThis()` | identical |  |
| ivp_3d_solver | `IVP_3D_Solver::calc_nullstelle(IVP_Time, IVP_Time, double, double, double, IVP_Real_Object*, IVP_Real_Object*)` | identical |  |
| ivp_3d_solver | `IVP_3D_Solver::find_first_t_for_value_coll(double, double, IVP_Time, IVP_Time, IVP_U_Matrix_Cache*, IVP_U_Matrix_Cache*, double*, IVP_Time*)` | identical |  |
| ivp_3d_solver | `IVP_3D_Solver::find_first_t_for_value_max_dev(double, IVP_Time, IVP_Time, int, IVP_U_Matrix_Cache*, IVP_U_Matrix_Cache*, double*, IVP_Time*)` | identical |  |
| ivp_actuator | `IVP_Actuator_Torque::IVP_Actuator_Torque(IVP_Environment*, IVP_Template_Torque*)` | differs | data ref at +0x71 (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_c |
| ivp_actuator | `IVP_Anchor::init_anchor(IVP_Actuator*, IVP_Template_Anchor*)` | identical |  |
| ivp_actuator | `IVP_Actuator::get_controller_name()` | identical |  |
| ivp_actuator | `IVP_Actuator::~IVP_Actuator()` | identical |  |
| ivp_actuator | `IVP_Actuator::~IVP_Actuator()` | identical |  |
| ivp_actuator | `IVP_Actuator_Force::get_controller_name()` | identical |  |
| ivp_actuator | `IVP_Actuator_Force::do_simulation_controller(IVP_Event_Sim*, IVP_U_Vector<IVP_Core>*)` | identical |  |
| ivp_actuator | `IVP_Actuator_Force::set_force(double)` | identical |  |
| ivp_actuator | `IVP_Actuator_Force::IVP_Actuator_Force(IVP_Environment*, IVP_Template_Force*)` | identical |  |
| ivp_actuator | `IVP_Actuator_Force::~IVP_Actuator_Force()` | identical |  |
| ivp_actuator | `IVP_Actuator_Force::~IVP_Actuator_Force()` | identical |  |
| ivp_actuator | `IVP_Actuator_Torque::set_torque(double)` | identical |  |
| ivp_actuator | `IVP_Actuator_Torque::get_controller_name()` | identical |  |
| ivp_actuator | `IVP_Actuator_Torque::do_simulation_controller(IVP_Event_Sim*, IVP_U_Vector<IVP_Core>*)` | identical |  |
| ivp_actuator | `IVP_Actuator_Torque::~IVP_Actuator_Torque()` | identical |  |
| ivp_actuator | `IVP_Actuator_Torque::~IVP_Actuator_Torque()` | identical |  |
| ivp_actuator | `IVP_Template_Anchor::set_anchor_position_cs(IVP_Real_Object*, IVP_U_Float_Point const*)` | identical |  |
| ivp_actuator | `IVP_Template_Anchor::set_anchor_position_os(IVP_Real_Object*, IVP_U_Float_Point const*)` | identical |  |
| ivp_actuator | `IVP_Template_Anchor::set_anchor_position_os(IVP_Real_Object*, double, double, double)` | identical |  |
| ivp_actuator | `IVP_Actuator_Two_Point::ensure_actuator_in_simulation()` | identical |  |
| ivp_actuator | `IVP_Actuator_Two_Point::IVP_Actuator_Two_Point(IVP_Environment*, IVP_Template_Two_Point*, IVP_ACTUATOR_TYPE)` | edited |  |
| ivp_actuator | `IVP_Actuator_Two_Point::~IVP_Actuator_Two_Point()` | identical |  |
| ivp_actuator | `IVP_Actuator_Two_Point::~IVP_Actuator_Two_Point()` | identical |  |
| ivp_actuator | `IVP_Actuator_Four_Point::IVP_Actuator_Four_Point(IVP_Environment*, IVP_Template_Four_Point*, IVP_ACTUATOR_TYPE)` | identical |  |
| ivp_actuator | `IVP_Actuator_Four_Point::~IVP_Actuator_Four_Point()` | identical |  |
| ivp_actuator | `IVP_Actuator_Four_Point::~IVP_Actuator_Four_Point()` | identical |  |
| ivp_actuator | `IVP_Actuator_Stabilizer::set_stabi_constant(double)` | identical |  |
| ivp_actuator | `IVP_Actuator_Stabilizer::get_controller_name()` | identical |  |
| ivp_actuator | `IVP_Actuator_Stabilizer::do_simulation_controller(IVP_Event_Sim*, IVP_U_Vector<IVP_Core>*)` | identical |  |
| ivp_actuator | `IVP_Actuator_Stabilizer::IVP_Actuator_Stabilizer(IVP_Environment*, IVP_Template_Stabilizer*)` | identical |  |
| ivp_actuator | `IVP_Actuator_Stabilizer::~IVP_Actuator_Stabilizer()` | identical |  |
| ivp_actuator | `IVP_Actuator_Force_Active::active_float_changed(IVP_U_Active_Float*)` | identical |  |
| ivp_actuator | `IVP_Actuator_Force_Active::IVP_Actuator_Force_Active(IVP_Environment*, IVP_Template_Force*)` | identical |  |
| ivp_actuator | `IVP_Actuator_Force_Active::~IVP_Actuator_Force_Active()` | identical |  |
| ivp_actuator | `IVP_Actuator_Force_Active::~IVP_Actuator_Force_Active()` | identical |  |
| ivp_actuator | `IVP_Actuator_Torque_Active::active_float_changed(IVP_U_Active_Float*)` | identical |  |
| ivp_actuator | `IVP_Actuator_Torque_Active::IVP_Actuator_Torque_Active(IVP_Environment*, IVP_Template_Torque*)` | identical |  |
| ivp_actuator | `IVP_Actuator_Torque_Active::~IVP_Actuator_Torque_Active()` | identical |  |
| ivp_actuator | `IVP_Actuator_Torque_Active::~IVP_Actuator_Torque_Active()` | identical |  |
| ivp_actuator | `non-virtual thunk to IVP_Actuator_Force_Active::active_float_changed(IVP_U_Active_Float*)` | identical |  |
| ivp_actuator | `non-virtual thunk to IVP_Actuator_Torque_Active::active_float_changed(IVP_U_Active_Float*)` | identical |  |
| ivp_actuator | `IVP_Anchor::object_is_going_to_be_deleted_event(IVP_Real_Object*)` | identical |  |
| ivp_actuator | `IVP_Actuator::anchor_will_be_deleted_event(IVP_Anchor*)` | identical |  |
| ivp_actuator | `IVP_Actuator::core_is_going_to_be_deleted_event(IVP_Core*)` | identical |  |
| ivp_actuator | `IVP_Template_Force::IVP_Template_Force()` | identical |  |
| ivp_actuator | `IVP_Actuator_Torque::IVP_Actuator_Torque(IVP_Environment*, IVP_Template_Torque*)` | identical |  |
| ivp_actuator | `IVP_Template_Torque::IVP_Template_Torque()` | identical |  |
| ivp_actuator | `IVP_Actuator_Two_Point::~IVP_Actuator_Two_Point()` | identical |  |
| ivp_actuator | `IVP_Template_Two_Point::IVP_Template_Two_Point()` | identical |  |
| ivp_actuator | `IVP_Actuator_Four_Point::~IVP_Actuator_Four_Point()` | identical |  |
| ivp_actuator | `IVP_Actuator_Stabilizer::~IVP_Actuator_Stabilizer()` | identical |  |
| ivp_actuator | `IVP_Template_Stabilizer::IVP_Template_Stabilizer()` | identical |  |
| ivp_actuator | `IVP_Actuator_Force_Active::IVP_Actuator_Force_Active(IVP_Environment*, IVP_Template_Force*)` | identical |  |
| ivp_actuator | `IVP_Actuator_Torque_Active::IVP_Actuator_Torque_Active(IVP_Environment*, IVP_Template_Torque*)` | identical |  |
| ivp_actuator | `IVP_Actuator_Torque_Active::~IVP_Actuator_Torque_Active()` | identical |  |
| ivp_actuator_spring | `IVP_Actuator_Suspension::do_simulation_controller(IVP_Event_Sim*, IVP_U_Vector<IVP_Core>*)` | differs | size retail 701 vs ours 665 |
| ivp_actuator_spring | `IVP_Actuator_Spring_Active::active_float_changed(IVP_U_Active_Float*)` | differs | data ref at +0x34 (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_c |
| ivp_actuator_spring | `IVP_Actuator_Spring::get_controller_name()` | identical |  |
| ivp_actuator_spring | `IVP_Actuator_Spring::do_simulation_controller(IVP_Event_Sim*, IVP_U_Vector<IVP_Core>*)` | edited |  |
| ivp_actuator_spring | `IVP_Actuator_Spring::IVP_Actuator_Spring(IVP_Environment*, IVP_Template_Spring*, IVP_ACTUATOR_TYPE)` | identical |  |
| ivp_actuator_spring | `IVP_Actuator_Spring::~IVP_Actuator_Spring()` | identical |  |
| ivp_actuator_spring | `IVP_Actuator_Spring::~IVP_Actuator_Spring()` | identical |  |
| ivp_actuator_spring | `IVP_Actuator_Suspension::get_controller_name()` | identical |  |
| ivp_actuator_spring | `IVP_Actuator_Suspension::IVP_Actuator_Suspension(IVP_Environment*, IVP_Template_Suspension*)` | identical |  |
| ivp_actuator_spring | `IVP_Actuator_Suspension::~IVP_Actuator_Suspension()` | identical |  |
| ivp_actuator_spring | `IVP_Actuator_Suspension::~IVP_Actuator_Suspension()` | identical |  |
| ivp_actuator_spring | `IVP_Actuator_Spring_Active::IVP_Actuator_Spring_Active(IVP_Environment*, IVP_Template_Spring*)` | identical |  |
| ivp_actuator_spring | `IVP_Actuator_Spring_Active::~IVP_Actuator_Spring_Active()` | identical |  |
| ivp_actuator_spring | `IVP_Actuator_Spring_Active::~IVP_Actuator_Spring_Active()` | identical |  |
| ivp_actuator_spring | `IVP_Actuator::get_controller_priority()` | identical |  |
| ivp_actuator_spring | `IVP_Actuator::get_associated_controlled_cores()` | identical |  |
| ivp_actuator_spring | `IVP_Actuator_Spring::set_constant(double)` | identical |  |
| ivp_actuator_spring | `IVP_Actuator_Spring::set_len(double)` | identical |  |
| ivp_actuator_spring | `IVP_Actuator_Spring::set_damp(double)` | identical |  |
| ivp_actuator_spring | `IVP_Actuator_Spring::IVP_Actuator_Spring(IVP_Environment*, IVP_Template_Spring*, IVP_ACTUATOR_TYPE)` | identical |  |
| ivp_actuator_spring | `IVP_Template_Spring::IVP_Template_Spring()` | identical |  |
| ivp_actuator_spring | `IVP_Actuator_Suspension::set_max_body_force(float)` | identical |  |
| ivp_actuator_spring | `IVP_Actuator_Suspension::set_spring_damp_compression(float)` | identical |  |
| ivp_actuator_spring | `IVP_Template_Suspension::IVP_Template_Suspension()` | identical |  |
| ivp_actuator_spring | `IVP_Actuator_Spring_Active::IVP_Actuator_Spring_Active(IVP_Environment*, IVP_Template_Spring*)` | identical |  |
| ivp_actuator_spring | `IVP_Actuator_Spring_Active::~IVP_Actuator_Spring_Active()` | identical |  |
| ivp_actuator_spring | `non-virtual thunk to IVP_Actuator_Spring_Active::active_float_changed(IVP_U_Active_Float*)` | identical |  |
| ivp_anomaly_manager | `IVP_Anomaly_Manager::inter_penetration(IVP_Mindist*, IVP_Real_Object*, IVP_Real_Object*, double)` | differs | bytes |
| ivp_anomaly_manager | `IVP_Anomaly_Limits::IVP_Anomaly_Limits(IVP_BOOL)` | identical |  |
| ivp_anomaly_manager | `IVP_Anomaly_Limits::~IVP_Anomaly_Limits()` | identical |  |
| ivp_anomaly_manager | `IVP_Anomaly_Manager::max_velocity_exceeded(IVP_Anomaly_Limits*, IVP_Core*, IVP_U_Float_Point*)` | identical |  |
| ivp_anomaly_manager | `IVP_Anomaly_Manager::get_push_speed_penetration(IVP_Real_Object*, IVP_Real_Object*)` | edited |  |
| ivp_anomaly_manager | `IVP_Anomaly_Manager::max_angular_velocity_exceeded(IVP_Anomaly_Limits*, IVP_Core*, IVP_U_Float_Point*)` | identical |  |
| ivp_anomaly_manager | `IVP_Anomaly_Manager::solve_inter_penetration_simple(IVP_Real_Object*, IVP_Real_Object*, double)` | new |  |
| ivp_anomaly_manager | `IVP_Anomaly_Manager::IVP_Anomaly_Manager(IVP_BOOL)` | identical |  |
| ivp_anomaly_manager | `IVP_Anomaly_Manager::IVP_Anomaly_Manager(IVP_BOOL)` | identical |  |
| ivp_anomaly_manager | `IVP_Anomaly_Manager::~IVP_Anomaly_Manager()` | identical |  |
| ivp_anomaly_manager | `IVP_Anomaly_Limits::environment_will_be_deleted(IVP_Environment*)` | identical |  |
| ivp_anomaly_manager | `IVP_Anomaly_Limits::~IVP_Anomaly_Limits()` | identical |  |
| ivp_anomaly_manager | `IVP_Anomaly_Manager::environment_will_be_deleted(IVP_Environment*)` | identical |  |
| ivp_anomaly_manager | `IVP_Anomaly_Manager::max_collision_checks_exceeded(int)` | identical |  |
| ivp_anomaly_manager | `IVP_Anomaly_Manager::max_contacts_exceeded_check_freezing(IVP_Core**, int)` | identical |  |
| ivp_anomaly_manager | `IVP_Anomaly_Manager::max_collisions_exceeded_check_freezing(IVP_Anomaly_Limits*, IVP_Core*)` | identical |  |
| ivp_anomaly_manager | `IVP_Anomaly_Manager::~IVP_Anomaly_Manager()` | identical |  |
| ivp_anomaly_manager | `IVP_Anomaly_Manager::~IVP_Anomaly_Manager()` | identical |  |
| ivp_ball | `IVP_SurfaceManager_Ball::get_all_terminal_ledges(IVP_U_BigVector<IVP_Compact_Ledge>*)` | differs | data ref at +0x16 (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_i |
| ivp_ball | `IVP_SurfaceManager_Ball::insert_all_ledges_hitting_ray(IVP_Ray_Solver*, IVP_Real_Object*)` | differs | data ref at +0xe (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_in |
| ivp_ball | `IVP_SurfaceManager_Ball::get_rotation_inertia(IVP_U_Float_Point*) const` | differs | data ref at +0xe (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_in |
| ivp_ball | `IVP_SurfaceManager_Ball::get_radius_and_radius_dev_to_given_center(IVP_U_Float_Point const*, float*, float*) const` | differs | data ref at +0xe (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_in |
| ivp_ball | `_GLOBAL__I_a` | identical |  |
| ivp_ball | `IVP_SurfaceManager_Ball::get_all_ledges_within_radius(IVP_U_Point const*, double, IVP_Compact_Ledge const*, IVP_Real_Object*, IVP_Compact_Ledge const*` | identical |  |
| ivp_ball | `IVP_SurfaceManager_Ball::~IVP_SurfaceManager_Ball()` | identical |  |
| ivp_ball | `IVP_SurfaceManager_Ball::~IVP_SurfaceManager_Ball()` | identical |  |
| ivp_ball | `IVP_Ball::IVP_Ball(IVP_Cluster*, IVP_Template_Ball const*, IVP_Template_Real_Object const*, IVP_U_Quat const*, IVP_U_Point const*)` | identical |  |
| ivp_ball | `IVP_Ball::~IVP_Ball()` | identical |  |
| ivp_ball | `IVP_Ball::~IVP_Ball()` | identical |  |
| ivp_ball | `IVP_SurfaceManager_Ball::get_type()` | identical |  |
| ivp_ball | `IVP_Ball::IVP_Ball(IVP_Cluster*, IVP_Template_Ball const*, IVP_Template_Real_Object const*, IVP_U_Quat const*, IVP_U_Point const*)` | identical |  |
| ivp_ball | `IVP_SurfaceManager_Ball::get_mass_center(IVP_U_Float_Point*) const` | identical |  |
| ivp_ball | `IVP_SurfaceManager_Ball::get_single_convex() const` | identical |  |
| ivp_betterdebugmanager | `_GLOBAL__I_a` | identical |  |
| ivp_betterdebugmanager | `IVP_BetterDebugmanager::output_function(IVP_DEBUG_CLASS, char const*)` | identical |  |
| ivp_betterdebugmanager | `IVP_BetterDebugmanager::dprint(IVP_DEBUG_CLASS, char const*, ...)` | identical |  |
| ivp_betterdebugmanager | `IVP_BetterDebugmanager::~IVP_BetterDebugmanager()` | identical |  |
| ivp_betterdebugmanager | `IVP_BetterDebugmanager::~IVP_BetterDebugmanager()` | identical |  |
| ivp_betterdebugmanager | `IVP_BetterDebugmanager::is_debug_enabled(IVP_DEBUG_CLASS)` | identical |  |
| ivp_betterstatisticsmanager | `IVP_BetterStatisticsmanager::~IVP_BetterStatisticsmanager()` | identical |  |
| ivp_betterstatisticsmanager | `IVP_BetterStatisticsmanager::IVP_BetterStatisticsmanager()` | identical |  |
| ivp_betterstatisticsmanager | `IVP_BetterStatisticsmanager::~IVP_BetterStatisticsmanager()` | identical |  |
| ivp_buoyancy_solver | `ivp_core_get_surface_speed_os(IVP_Core*, IVP_Real_Object*, IVP_U_Float_Point const*, IVP_U_Float_Point*)` | identical |  |
| ivp_buoyancy_solver | `IVP_Buoyancy_Solver::compute_forces(IVP_U_Float_Point const*, IVP_U_Float_Hesse const*, IVP_Real_Object*)` | identical |  |
| ivp_buoyancy_solver | `IVP_Buoyancy_Solver::compute_values_for_one_ball(IVP_Real_Object const*, IVP_U_Float_Hesse const*, IVP_U_Float_Point const*)` | identical |  |
| ivp_buoyancy_solver | `IVP_Buoyancy_Solver::compute_values_for_one_polygon(IVP_Real_Object*, IVP_U_Float_Hesse const*)` | identical |  |
| ivp_buoyancy_solver | `IVP_Buoyancy_Solver::compute_values_for_one_triangle(IVP_Real_Object*, IVP_Compact_Triangle const*, IVP_U_Float_Hesse const*, IVP_U_Float_Point const*` | identical |  |
| ivp_buoyancy_solver | `IVP_Buoyancy_Solver::compute_disection_points_with_ball(IVP_U_Float_Hesse const*, IVP_U_Float_Hesse const*, IVP_U_Float_Point const*, float const&, IV` | identical |  |
| ivp_buoyancy_solver | `IVP_Buoyancy_Solver::compute_buoyancy_values_for_one_ball(int const&, float const&, float const&, IVP_U_Float_Hesse const*, IVP_U_Float_Point const*)` | identical |  |
| ivp_buoyancy_solver | `IVP_Buoyancy_Solver::compute_dampening_values_for_one_ball(int const&, float const&, float const&, IVP_U_Float_Point const*, IVP_U_Float_Point const*,` | identical |  |
| ivp_buoyancy_solver | `IVP_Buoyancy_Solver::compute_volumes_and_centers_for_one_pyramid(IVP_Real_Object*, IVP_U_Float_Point const**, float const*, int const&, int const&, in` | identical |  |
| ivp_buoyancy_solver | `IVP_Buoyancy_Solver::compute_rotation_and_translation_values_for_one_triangle(IVP_Real_Object*, IVP_Compact_Triangle const*, IVP_U_Float_Point const**` | identical |  |
| ivp_buoyancy_solver | `IVP_Buoyancy_Solver::IVP_Buoyancy_Solver(IVP_Core*, IVP_Controller_Buoyancy*, IVP_Template_Buoyancy const*, IVP_U_Float_Point const*)` | identical |  |
| ivp_cache_object | `IVP_Cache_Object::update_cache_object()` | identical |  |
| ivp_cache_object | `IVP_Cache_Object_Manager::IVP_Cache_Object_Manager(int)` | edited |  |
| ivp_cache_object | `IVP_Cache_Object_Manager::~IVP_Cache_Object_Manager()` | edited |  |
| ivp_cache_object | `IVP_Cache_Object_Manager::get_cache_object(IVP_Real_Object*)` | identical |  |
| ivp_cache_object | `IVP_Cache_Object_Manager::invalid_cache_object(IVP_Real_Object*)` | identical |  |
| ivp_cache_object | `IVP_Cache_Object::transform_vector_to_world_coords(IVP_U_Point const*, IVP_U_Point*) const` | identical |  |
| ivp_cache_object | `IVP_Cache_Object::transform_vector_to_world_coords(IVP_U_Float_Point const*, IVP_U_Float_Point*) const` | identical |  |
| ivp_cache_object | `IVP_Cache_Object::transform_vector_to_object_coords(IVP_U_Point const*, IVP_U_Point*) const` | identical |  |
| ivp_cache_object | `IVP_Cache_Object::transform_vector_to_object_coords(IVP_U_Float_Point const*, IVP_U_Float_Point*) const` | identical |  |
| ivp_cache_object | `IVP_Cache_Object::transform_position_to_world_coords(IVP_U_Float_Point const*, IVP_U_Point*) const` | identical |  |
| ivp_cache_object | `IVP_Cache_Object::transform_position_to_object_coords(IVP_U_Point const*, IVP_U_Point*) const` | identical |  |
| ivp_calc_next_psi_solver | `IVP_Calc_Next_PSI_Solver::set_transformation(IVP_U_Quat const*, IVP_U_Point const*, IVP_BOOL)` | differs | size retail 1534 vs ours 1518 |
| ivp_calc_next_psi_solver | `IVP_Calc_Next_PSI_Solver::commit_all_hull_managers(IVP_Environment*, IVP_U_Vector<IVP_Hull_Manager_Base>*)` | differs | size retail 640 vs ours 642 |
| ivp_calc_next_psi_solver | `IVP_Calc_Next_PSI_Solver::calc_next_PSI_matrix(IVP_Event_Sim*, IVP_U_Vector<IVP_Hull_Manager_Base>*)` | edited |  |
| ivp_calc_next_psi_solver | `IVP_Calc_Next_PSI_Solver::calc_rotation_matrix(float, IVP_U_Quat*)` | identical |  |
| ivp_calc_next_psi_solver | `IVP_Calc_Next_PSI_Solver::commit_all_calc_next_PSI_matrix(IVP_Environment*, IVP_U_Vector<IVP_Core>*, IVP_U_Vector<IVP_Hull_Manager_Base>*)` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::IVP_Car_System_Real_Wheels(IVP_Environment*, IVP_Template_Car_System*)` | differs | bytes |
| ivp_car_system | `IVP_Car_System::IVP_Car_System()` | identical |  |
| ivp_car_system | `IVP_Car_System::~IVP_Car_System()` | identical |  |
| ivp_car_system | `IVP_Car_System::~IVP_Car_System()` | identical |  |
| ivp_car_system | `IVP_Car_System::~IVP_Car_System()` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::do_steering(float, bool)` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::set_powerslide(float, float)` | edited |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::activate_booster(float, float, float)` | edited |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::do_steering_wheel(IVP_POS_WHEEL, float)` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::change_wheel_torque(IVP_POS_WHEEL, float)` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::change_spring_length(IVP_POS_WHEEL, float)` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::GetCarSystemDebugData(IVP_CarSystemDebugData_t&)` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::change_body_downforce(float)` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::change_max_body_force(IVP_POS_WHEEL, float)` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::change_spring_constant(IVP_POS_WHEEL, float)` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::change_spring_dampening(IVP_POS_WHEEL, float)` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::get_orig_axles_distance()` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::set_booster_acceleration(float)` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::change_spring_pre_tension(IVP_POS_WHEEL, float)` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::update_body_countertorque()` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::change_stabilizer_constant(IVP_POS_AXIS, float)` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::change_spring_dampening_compression(IVP_POS_WHEEL, float)` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::fix_wheel(IVP_POS_WHEEL, IVP_BOOL)` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::~IVP_Car_System_Real_Wheels()` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::~IVP_Car_System_Real_Wheels()` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::get_skid_info(IVP_Wheel_Skid_Info*)` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::get_body_speed(IVP_COORDINATE_INDEX)` | edited |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::update_booster(float)` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::update_throttle(float)` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::get_wheel_torque(IVP_POS_WHEEL)` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::get_booster_delay()` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::SetCarSystemDebugData(IVP_CarSystemDebugData_t const&)` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::get_booster_time_to_go()` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::update_wheel_positions()` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::change_fast_turn_factor(float)` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::get_wheel_angular_velocity(IVP_POS_WHEEL)` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::environment_will_be_deleted(IVP_Environment*)` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::get_orig_front_wheel_distance()` | identical |  |
| ivp_car_system | `IVP_Car_System_Real_Wheels::IVP_Car_System_Real_Wheels(IVP_Environment*, IVP_Template_Car_System*)` | identical |  |
| ivp_clustering_longrange | `IVP_OV_Tree_Manager::calc_optimal_box(IVP_OV_Element const*, double, double)` | differs | size retail 898 vs ours 867 |
| ivp_clustering_longrange | `IVP_OV_Node::~IVP_OV_Node()` | identical |  |
| ivp_clustering_longrange | `IVP_OV_Element::add_oo_collision(IVP_Collision*)` | identical |  |
| ivp_clustering_longrange | `IVP_OV_Element::add_to_hull_manager(IVP_Hull_Manager*, double)` | identical |  |
| ivp_clustering_longrange | `IVP_OV_Element::hull_limit_exceeded_event(IVP_Hull_Manager*, float)` | identical |  |
| ivp_clustering_longrange | `IVP_OV_Element::IVP_OV_Element(IVP_Real_Object*)` | identical |  |
| ivp_clustering_longrange | `IVP_OV_Element::~IVP_OV_Element()` | identical |  |
| ivp_clustering_longrange | `IVP_OV_Element::~IVP_OV_Element()` | identical |  |
| ivp_clustering_longrange | `IVP_OV_Tree_Manager::expand_tree(IVP_OV_Node const*)` | edited |  |
| ivp_clustering_longrange | `IVP_OV_Tree_Manager::connect_boxes(IVP_OV_Node*, IVP_OV_Node*)` | edited |  |
| ivp_clustering_longrange | `IVP_OV_Tree_Manager::insert_ov_element(IVP_OV_Element*, double, double, IVP_U_Vector<IVP_OV_Element>*)` | edited |  |
| ivp_clustering_longrange | `IVP_OV_Tree_Manager::remove_ov_element(IVP_OV_Element*)` | edited |  |
| ivp_clustering_longrange | `IVP_OV_Tree_Manager::get_luf_coordinates_ws(IVP_OV_Node const*, IVP_U_Float_Point*, float*)` | edited |  |
| ivp_clustering_longrange | `IVP_OV_Tree_Manager::collect_collision_partners(IVP_OV_Element const*, IVP_OV_Node const*, IVP_OV_Node const*)` | edited |  |
| ivp_clustering_longrange | `IVP_OV_Tree_Manager::collect_subbox_collision_partners(IVP_OV_Element const*, IVP_OV_Node const*)` | edited |  |
| ivp_clustering_longrange | `IVP_OV_Tree_Manager::IVP_OV_Tree_Manager()` | edited |  |
| ivp_clustering_longrange | `IVP_OV_Element::remove_oo_collision(IVP_Collision*)` | identical |  |
| ivp_clustering_longrange | `IVP_OV_Element::hull_manager_is_going_to_be_deleted_event(IVP_Hull_Manager*)` | identical |  |
| ivp_clustering_longrange | `IVP_OV_Element::get_type()` | identical |  |
| ivp_clustering_longrange | `IVP_OV_Element::~IVP_OV_Element()` | identical |  |
| ivp_clustering_longrange | `IVP_OV_Tree_Manager::IVP_OV_Tree_Manager()` | identical |  |
| ivp_clustering_longrange | `IVP_OV_Tree_Manager::~IVP_OV_Tree_Manager()` | edited |  |
| ivp_clustering_lrange_hash | `IVP_ov_tree_hash::node_to_index(IVP_OV_Node*)` | identical |  |
| ivp_clustering_lrange_hash | `IVP_ov_tree_hash::~IVP_ov_tree_hash()` | identical |  |
| ivp_clustering_lrange_hash | `IVP_ov_tree_hash::~IVP_ov_tree_hash()` | identical |  |
| ivp_clustering_lrange_hash | `IVP_ov_tree_hash::compare(void*, void*) const` | identical |  |
| ivp_coll_del_root_mindist | `IVP_Collision_Delegator::~IVP_Collision_Delegator()` | identical |  |
| ivp_coll_del_root_mindist | `IVP_Collision_Delegator::~IVP_Collision_Delegator()` | identical |  |
| ivp_coll_del_root_mindist | `IVP_Collision_Delegator_Root_Mindist::delegate_collisions_for_object(IVP_Real_Object*, IVP_Real_Object*)` | identical |  |
| ivp_coll_del_root_mindist | `IVP_Collision_Delegator_Root_Mindist::collision_is_going_to_be_deleted_event(IVP_Collision*)` | identical |  |
| ivp_coll_del_root_mindist | `IVP_Collision_Delegator_Root_Mindist::IVP_Collision_Delegator_Root_Mindist()` | identical |  |
| ivp_coll_del_root_mindist | `IVP_Collision_Delegator_Root_Mindist::~IVP_Collision_Delegator_Root_Mindist()` | identical |  |
| ivp_coll_del_root_mindist | `IVP_Collision_Delegator_Root_Mindist::~IVP_Collision_Delegator_Root_Mindist()` | identical |  |
| ivp_coll_del_root_mindist | `IVP_Collision_Delegator::get_spawned_mindist_count()` | identical |  |
| ivp_coll_del_root_mindist | `IVP_Collision_Delegator::change_spawned_mindist_count(int)` | identical |  |
| ivp_coll_del_root_mindist | `IVP_Collision_Delegator_Root_Mindist::environment_is_going_to_be_deleted_event(IVP_Environment*)` | identical |  |
| ivp_coll_del_root_mindist | `IVP_Collision_Delegator_Root_Mindist::object_is_removed_from_collision_detection(IVP_Real_Object*)` | identical |  |
| ivp_collision_filter | `IVP_CFEP_Hash::~IVP_CFEP_Hash()` | identical |  |
| ivp_collision_filter | `IVP_CFEP_Hash::~IVP_CFEP_Hash()` | identical |  |
| ivp_collision_filter | `IVP_Collision_Filter::~IVP_Collision_Filter()` | edited |  |
| ivp_collision_filter | `IVP_Collision_Filter_Exclusive_Pair::enable_collision_between_objects(IVP_Real_Object*, IVP_Real_Object*)` | identical |  |
| ivp_collision_filter | `IVP_Collision_Filter_Exclusive_Pair::disable_collision_between_objects(IVP_Real_Object*, IVP_Real_Object*)` | edited |  |
| ivp_collision_filter | `IVP_Collision_Filter_Exclusive_Pair::check_objects_for_collision_detection(IVP_Real_Object*, IVP_Real_Object*)` | identical |  |
| ivp_collision_filter | `IVP_Collision_Filter_Exclusive_Pair::IVP_Collision_Filter_Exclusive_Pair()` | edited |  |
| ivp_collision_filter | `IVP_Collision_Filter_Exclusive_Pair::~IVP_Collision_Filter_Exclusive_Pair()` | edited |  |
| ivp_collision_filter | `IVP_Collision_Filter_Exclusive_Pair::~IVP_Collision_Filter_Exclusive_Pair()` | edited |  |
| ivp_collision_filter | `IVP_Collision_Filter_Exclusive_Pair::~IVP_Collision_Filter_Exclusive_Pair()` | identical |  |
| ivp_collision_filter | `IVP_Collision_Filter::~IVP_Collision_Filter()` | edited |  |
| ivp_collision_filter | `IVP_Collision_Filter::~IVP_Collision_Filter()` | identical |  |
| ivp_collision_filter | `IVP_Collision_Filter_Exclusive_Pair::environment_will_be_deleted(IVP_Environment*)` | identical |  |
| ivp_collision_filter | `IVP_CFEP_Hash::compare(void*, void*) const` | identical |  |
| ivp_compact_ledge | `IVP_Compact_Ledge::c_ledge_init()` | identical |  |
| ivp_compact_ledge | `IVP_Compact_Triangle::IVP_Compact_Triangle()` | identical |  |
| ivp_compact_ledge | `IVP_Compact_Ledgetree_Node::byte_swap_all(IVP_U_BigVector<IVP_Compact_Poly_Point>*)` | identical |  |
| ivp_compact_ledge_gen | `IVP_Compact_Ledge_Generator::prepare_compact_ledge(IVP_U_Vector<IVP_Triangle>*)` | identical |  |
| ivp_compact_ledge_gen | `IVP_Compact_Ledge_Generator::generate_compact_ledge(unsigned char*)` | identical |  |
| ivp_compact_ledge_gen | `IVP_Compact_Ledge_Generator::~IVP_Compact_Ledge_Generator()` | identical |  |
| ivp_compact_ledge_gen | `IVP_Compact_Ledge_Generator::IVP_Compact_Ledge_Generator()` | identical |  |
| ivp_compact_ledge_gen | `IVP_Compact_Ledge_Generator::~IVP_Compact_Ledge_Generator()` | identical |  |
| ivp_compact_ledge_solver | `IVP_KK_Input::IVP_KK_Input(IVP_Compact_Edge const*, IVP_Compact_Edge const*, IVP_Cache_Ledge_Point*, IVP_Cache_Ledge_Point*)` | differs | size retail 1071 vs ours 1160 |
| ivp_compact_ledge_solver | `IVP_Compact_Ledge_Solver::calc_qlen_KK(IVP_Compact_Edge const*, IVP_Compact_Edge const*, IVP_Cache_Ledge_Point*, IVP_Cache_Ledge_Point*)` | differs | data ref at +0xd9 (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_c |
| ivp_compact_ledge_solver | `IVP_Compact_Ledge_Solver::calc_qlen_PF_F_space(IVP_Compact_Ledge const*, IVP_Compact_Triangle const*, IVP_U_Point const*)` | differs | size retail 1227 vs ours 1203 |
| ivp_compact_ledge_solver | `IVP_Compact_Ledge_Solver::calc_qlen_PK_K_space(IVP_U_Point const*, IVP_Compact_Ledge const*, IVP_Compact_Edge const*)` | differs | size retail 692 vs ours 685 |
| ivp_compact_ledge_solver | `IVP_Compact_Ledge_Solver::calc_unscaled_KK_vals(IVP_KK_Input const&, IVP_Unscaled_KK_Result*)` | differs | size retail 2821 vs ours 2669 |
| ivp_compact_ledge_solver | `IVP_Compact_Ledge_Solver::calc_unscaled_s_val_K_space(IVP_Compact_Ledge const*, IVP_Compact_Edge const*, IVP_U_Point const*, IVP_Unscaled_S_Result*)` | differs | size retail 234 vs ours 213 |
| ivp_compact_ledge_solver | `IVP_Compact_Ledge_Solver::calc_unscaled_qr_vals_F_space(IVP_Compact_Ledge const*, IVP_Compact_Edge const*, IVP_U_Point const*, IVP_Unscaled_QR_Result*` | differs | size retail 514 vs ours 488 |
| ivp_compact_ledge_solver | `IVP_Compact_Ledge_Solver::quad_dist_edge_to_point_K_space(IVP_Compact_Ledge const*, IVP_Compact_Edge const*, IVP_U_Point const*)` | differs | size retail 341 vs ours 333 |
| ivp_compact_ledge_solver | `IVP_KK_Input::calc_quad_distance_edge_edge()` | identical |  |
| ivp_compact_ledge_solver | `IVP_KK_Input::IVP_KK_Input(IVP_Compact_Edge const*, IVP_Compact_Edge const*, IVP_Cache_Ledge_Point*, IVP_Cache_Ledge_Point*)` | identical |  |
| ivp_compact_ledge_solver | `IVP_Compact_Ledge_Solver::get_all_ledges(IVP_Compact_Ledgetree_Node const*, IVP_U_BigVector<IVP_Compact_Ledge>*)` | identical |  |
| ivp_compact_ledge_solver | `IVP_Compact_Ledge_Solver::calc_bounding_box(IVP_Compact_Ledge const*, IVP_U_Point*, IVP_U_Point*)` | identical |  |
| ivp_compact_ledge_solver | `IVP_Compact_Ledge_Solver::calc_hesse_object(IVP_Compact_Edge const*, IVP_Compact_Ledge const*, IVP_U_Hesse*)` | identical |  |
| ivp_compact_ledge_solver | `IVP_Compact_Ledge_Solver::give_world_coords_AT(IVP_Compact_Edge const*, IVP_Cache_Ledge_Point*, IVP_U_Point*)` | identical |  |
| ivp_compact_ledge_solver | `IVP_Compact_Ledge_Solver::calc_radius_to_given_center(IVP_Compact_Ledge const*, IVP_U_Point const*, double*, double*)` | identical |  |
| ivp_compact_ledge_solver | `IVP_Compact_Ledge_Solver::calc_radius_to_given_center(IVP_Compact_Surface const*, IVP_U_Point const*, double*, double*)` | identical |  |
| ivp_compact_ledge_solver | `IVP_Compact_Ledge_Solver::calc_hesse_vec_object_not_normized(IVP_Compact_Edge const*, IVP_Compact_Ledge const*, IVP_U_Point*)` | identical |  |
| ivp_compact_ledge_solver | `IVP_Compact_Ledge_Solver::calc_hesse_vec_object_not_normized(IVP_Compact_Edge const*, IVP_Compact_Ledge const*, IVP_U_Float_Point*)` | identical |  |
| ivp_compact_ledge_solver | `IVP_Compact_Ledge_Solver::get_all_ledges(IVP_Compact_Surface const*, IVP_U_BigVector<IVP_Compact_Ledge>*)` | identical |  |
| ivp_compact_ledge_solver | `IVP_Compact_Ledge_Solver::calc_pos_other_space(IVP_Compact_Edge const*, IVP_Cache_Ledge_Point*, IVP_Cache_Ledge_Point*, IVP_U_Point*)` | identical |  |
| ivp_compact_ledge_solver | `IVP_Compact_Ledge_Solver::transform_pos_other_space(IVP_U_Float_Point const*, IVP_Cache_Ledge_Point*, IVP_Cache_Ledge_Point*, IVP_U_Point*)` | identical |  |
| ivp_compact_recursive | `IVP_Compact_Recursive::add_compact_ledge(IVP_Compact_Ledge const*)` | identical |  |
| ivp_compact_recursive | `IVP_Compact_Recursive::build_convex_hull()` | identical |  |
| ivp_compact_recursive | `IVP_Compact_Recursive::set_rekursive_convex_hull()` | identical |  |
| ivp_compact_recursive | `IVP_Compact_Recursive::IVP_Compact_Recursive()` | identical |  |
| ivp_compact_recursive | `IVP_Compact_Recursive::~IVP_Compact_Recursive()` | identical |  |
| ivp_compact_recursive | `IVP_Compact_Recursive::compile()` | identical |  |
| ivp_compact_surface | `IVP_Compact_Surface::byte_swap_all(IVP_BOOL, int)` | differs | size retail 206 vs ours 265 |
| ivp_constraint | `IVP_Constraint::deactivate()` | identical |  |
| ivp_constraint | `IVP_Constraint::fix_rotation_axis(IVP_COORDINATE_INDEX)` | identical |  |
| ivp_constraint | `IVP_Constraint::free_rotation_axis(IVP_COORDINATE_INDEX)` | identical |  |
| ivp_constraint | `IVP_Constraint::limit_rotation_axis(IVP_COORDINATE_INDEX, float, float)` | identical |  |
| ivp_constraint | `IVP_Constraint::fix_translation_axis(IVP_COORDINATE_INDEX)` | identical |  |
| ivp_constraint | `IVP_Constraint::free_translation_axis(IVP_COORDINATE_INDEX)` | identical |  |
| ivp_constraint | `IVP_Constraint::limit_translation_axis(IVP_COORDINATE_INDEX, float, float)` | identical |  |
| ivp_constraint | `IVP_Constraint::change_fixing_point_Ros(IVP_U_Point const*)` | identical |  |
| ivp_constraint | `IVP_Constraint::change_rotation_axes_Ros(IVP_U_Matrix3 const*)` | identical |  |
| ivp_constraint | `IVP_Constraint::change_max_rotation_impulse(IVP_CONSTRAINT_FORCE_EXCEED, float)` | identical |  |
| ivp_constraint | `IVP_Constraint::change_translation_axes_Ros(IVP_U_Matrix3 const*)` | identical |  |
| ivp_constraint | `IVP_Constraint::change_max_translation_impulse(IVP_CONSTRAINT_FORCE_EXCEED, float)` | identical |  |
| ivp_constraint | `IVP_Constraint::change_target_fixing_point_Ros(IVP_U_Point const*)` | identical |  |
| ivp_constraint | `IVP_Constraint::change_Aos_to_relaxe_constraint()` | identical |  |
| ivp_constraint | `IVP_Constraint::change_Ros_to_relaxe_constraint()` | identical |  |
| ivp_constraint | `IVP_Constraint::change_target_rotation_axes_Ros(IVP_U_Matrix3 const*)` | identical |  |
| ivp_constraint | `IVP_Constraint::change_target_translation_axes_Ros(IVP_U_Matrix3 const*)` | identical |  |
| ivp_constraint | `IVP_Constraint::activate()` | identical |  |
| ivp_constraint | `IVP_Constraint::IVP_Constraint()` | identical |  |
| ivp_constraint | `IVP_Constraint::~IVP_Constraint()` | identical |  |
| ivp_constraint | `IVP_Constraint::~IVP_Constraint()` | identical |  |
| ivp_constraint | `IVP_Constraint::get_environment()` | identical |  |
| ivp_constraint | `IVP_Constraint::get_minimum_simulation_frequency()` | identical |  |
| ivp_constraint | `IVP_Constraint::core_is_going_to_be_deleted_event(IVP_Core*)` | identical |  |
| ivp_constraint | `IVP_Constraint::~IVP_Constraint()` | identical |  |
| ivp_constraint_car | `IVP_Constraint_Solver_Car::do_simulation_controller(IVP_Event_Sim*, IVP_U_Vector<IVP_Core>*)` | differs | size retail 1645 vs ours 1751 |
| ivp_constraint_car | `IVP_Constraint_Car_Object::IVP_Constraint_Car_Object(IVP_Constraint_Solver_Car*, IVP_Real_Object*, IVP_Real_Object*, IVP_U_Float_Point*)` | edited |  |
| ivp_constraint_car | `IVP_Constraint_Solver_Car::get_controller_name()` | identical |  |
| ivp_constraint_car | `IVP_Constraint_Solver_Car::init_constraint_system(IVP_Environment*, IVP_Real_Object*, IVP_U_Vector<IVP_Real_Object>&, IVP_U_Vector<IVP_U_Float_Point>&` | edited |  |
| ivp_constraint_car | `IVP_Constraint_Solver_Car::get_minimum_simulation_frequency()` | identical |  |
| ivp_constraint_car | `IVP_Constraint_Solver_Car::do_simulation_controller_rotation(IVP_Event_Sim*, IVP_Core*, IVP_U_Matrix const*)` | identical |  |
| ivp_constraint_car | `IVP_Constraint_Solver_Car::IVP_Constraint_Solver_Car(IVP_COORDINATE_INDEX, IVP_COORDINATE_INDEX, IVP_COORDINATE_INDEX, IVP_BOOL)` | identical |  |
| ivp_constraint_car | `IVP_Constraint_Solver_Car::~IVP_Constraint_Solver_Car()` | edited |  |
| ivp_constraint_car | `IVP_Constraint_Solver_Car::~IVP_Constraint_Solver_Car()` | identical |  |
| ivp_constraint_car | `IVP_Constraint_Solver_Car_Builder::calc_pushing_behavior(int, int)` | identical |  |
| ivp_constraint_car | `IVP_Constraint_Solver_Car_Builder::calc_constraint_matrix()` | identical |  |
| ivp_constraint_car | `IVP_Constraint_Solver_Car::get_controller_priority()` | identical |  |
| ivp_constraint_car | `IVP_Constraint_Solver_Car::get_associated_controlled_cores()` | identical |  |
| ivp_constraint_car | `IVP_Constraint_Solver_Car::core_is_going_to_be_deleted_event(IVP_Core*)` | edited |  |
| ivp_constraint_car | `IVP_Constraint_Solver_Car::~IVP_Constraint_Solver_Car()` | identical |  |
| ivp_constraint_local | `IVP_Constraint_Local::do_simulation_controller(IVP_Event_Sim*, IVP_U_Vector<IVP_Core>*)` | differs | size retail 8014 vs ours 8049 |
| ivp_constraint_local | `IVP_Constraint_Local::fix_rotation_axis(IVP_COORDINATE_INDEX)` | identical |  |
| ivp_constraint_local | `IVP_Constraint_Local::free_rotation_axis(IVP_COORDINATE_INDEX)` | identical |  |
| ivp_constraint_local | `IVP_Constraint_Local::limit_rotation_axis(IVP_COORDINATE_INDEX, float, float)` | identical |  |
| ivp_constraint_local | `IVP_Constraint_Local::fix_translation_axis(IVP_COORDINATE_INDEX)` | identical |  |
| ivp_constraint_local | `IVP_Constraint_Local::free_translation_axis(IVP_COORDINATE_INDEX)` | identical |  |
| ivp_constraint_local | `IVP_Constraint_Local::limit_translation_axis(IVP_COORDINATE_INDEX, float, float)` | identical |  |
| ivp_constraint_local | `IVP_Constraint_Local::change_fixing_point_Ros(IVP_U_Point const*)` | identical |  |
| ivp_constraint_local | `IVP_Constraint_Local::change_rotation_axes_Ros(IVP_U_Matrix3 const*)` | identical |  |
| ivp_constraint_local | `IVP_Constraint_Local::change_max_rotation_impulse(IVP_CONSTRAINT_FORCE_EXCEED, float)` | identical |  |
| ivp_constraint_local | `IVP_Constraint_Local::change_translation_axes_Ros(IVP_U_Matrix3 const*)` | identical |  |
| ivp_constraint_local | `IVP_Constraint_Local::change_max_translation_impulse(IVP_CONSTRAINT_FORCE_EXCEED, float)` | identical |  |
| ivp_constraint_local | `IVP_Constraint_Local::change_target_fixing_point_Ros(IVP_U_Point const*)` | identical |  |
| ivp_constraint_local | `IVP_Constraint_Local::change_Aos_to_relaxe_constraint()` | identical |  |
| ivp_constraint_local | `IVP_Constraint_Local::change_Ros_to_relaxe_constraint()` | identical |  |
| ivp_constraint_local | `IVP_Constraint_Local::change_target_rotation_axes_Ros(IVP_U_Matrix3 const*)` | identical |  |
| ivp_constraint_local | `IVP_Constraint_Local::change_target_translation_axes_Ros(IVP_U_Matrix3 const*)` | identical |  |
| ivp_constraint_local | `IVP_Constraint_Local::init(IVP_Template_Constraint const&)` | identical |  |
| ivp_constraint_local | `IVP_Constraint_Local::IVP_Constraint_Local(IVP_Template_Constraint const&)` | identical |  |
| ivp_constraint_local | `IVP_Constraint_Local::~IVP_Constraint_Local()` | identical |  |
| ivp_constraint_local | `IVP_Constraint_Local::~IVP_Constraint_Local()` | identical |  |
| ivp_constraint_local | `IVP_Constraint::get_controller_priority()` | identical |  |
| ivp_constraint_local | `IVP_Constraint::get_associated_controlled_cores()` | identical |  |
| ivp_constraint_local | `IVP_Constraint_Local::sort_rotation_mapping()` | identical |  |
| ivp_constraint_local | `IVP_Constraint_Local::sort_translation_mapping()` | identical |  |
| ivp_constraint_local | `IVP_Constraint_Local::core_is_going_to_be_deleted_event(IVP_Core*)` | identical |  |
| ivp_controller_buoyancy | `IVP_Controller_Buoyancy::do_simulation_controller(IVP_Event_Sim*, IVP_U_Vector<IVP_Core>*)` | differs | size retail 1441 vs ours 1457 |
| ivp_controller_buoyancy | `IVP_Controller_Buoyancy::apply_dampening(IVP_Real_Object*, float, double, IVP_U_Float_Point*, IVP_U_Float_Point*, IVP_U_Float_Point*)` | identical |  |
| ivp_controller_buoyancy | `IVP_Controller_Buoyancy::get_controller_name()` | identical |  |
| ivp_controller_buoyancy | `IVP_Controller_Buoyancy::use_buoyancy_solver(IVP_Buoyancy_Input const*, IVP_Template_Buoyancy const*, IVP_Buoyancy_Output*, IVP_U_Float_Point const*, ` | identical |  |
| ivp_controller_buoyancy | `IVP_Controller_Buoyancy::apply_buoyancy_impulse(IVP_Real_Object*, IVP_Template_Buoyancy*, double, float, IVP_U_Float_Point*)` | identical |  |
| ivp_controller_buoyancy | `IVP_Controller_Buoyancy::IVP_Controller_Buoyancy(IVP_Attacher_To_Cores<IVP_Controller_Buoyancy>*, IVP_Core*)` | identical |  |
| ivp_controller_buoyancy | `IVP_Controller_Buoyancy::~IVP_Controller_Buoyancy()` | identical |  |
| ivp_controller_buoyancy | `IVP_Controller_Buoyancy::~IVP_Controller_Buoyancy()` | identical |  |
| ivp_controller_buoyancy | `IVP_Attacher_To_Cores_Buoyancy::IVP_Attacher_To_Cores_Buoyancy(IVP_Template_Buoyancy&, IVP_U_Set_Active<IVP_Core>*, IVP_Liquid_Surface_Descriptor*)` | identical |  |
| ivp_controller_buoyancy | `IVP_Attacher_To_Cores_Buoyancy::~IVP_Attacher_To_Cores_Buoyancy()` | identical |  |
| ivp_controller_buoyancy | `IVP_Attacher_To_Cores_Buoyancy::~IVP_Attacher_To_Cores_Buoyancy()` | identical |  |
| ivp_controller_buoyancy | `IVP_Controller_Buoyancy::get_controller_priority()` | identical |  |
| ivp_controller_buoyancy | `IVP_Controller_Buoyancy::get_minimum_simulation_frequency()` | identical |  |
| ivp_controller_buoyancy | `IVP_Controller_Buoyancy::core_is_going_to_be_deleted_event(IVP_Core*)` | identical |  |
| ivp_controller_buoyancy | `IVP_Attacher_To_Cores_Buoyancy::get_parameters_per_core(IVP_Core*)` | identical |  |
| ivp_controller_phantom | `IVP_Controller_Phantom::~IVP_Controller_Phantom()` | differs | size retail 673 vs ours 671 |
| ivp_controller_phantom | `IVP_Controller_Phantom::mindist_left_volume(IVP_Mindist*)` | identical |  |
| ivp_controller_phantom | `IVP_Controller_Phantom::add_listener_phantom(IVP_Listener_Phantom*)` | identical |  |
| ivp_controller_phantom | `IVP_Controller_Phantom::event_object_revived(IVP_Event_Object*)` | identical |  |
| ivp_controller_phantom | `IVP_Controller_Phantom::mindist_entered_volume(IVP_Mindist*)` | identical |  |
| ivp_controller_phantom | `IVP_Controller_Phantom::wake_all_sleeping_objects()` | identical |  |
| ivp_controller_phantom | `IVP_Controller_Phantom::IVP_Controller_Phantom(IVP_Real_Object*, IVP_Template_Phantom const*)` | identical |  |
| ivp_controller_phantom | `IVP_Controller_Phantom::~IVP_Controller_Phantom()` | identical |  |
| ivp_controller_phantom | `IVP_Controller_Phantom::~IVP_Controller_Phantom()` | identical |  |
| ivp_controller_phantom | `IVP_Template_Phantom::IVP_Template_Phantom()` | identical |  |
| ivp_controller_phantom | `IVP_Controller_Phantom::event_object_frozen(IVP_Event_Object*)` | identical |  |
| ivp_controller_phantom | `IVP_Controller_Phantom::event_object_created(IVP_Event_Object*)` | identical |  |
| ivp_controller_phantom | `IVP_Controller_Phantom::event_object_deleted(IVP_Event_Object*)` | identical |  |
| ivp_controller_phantom | `IVP_Controller_Phantom::remove_listener_phantom(IVP_Listener_Phantom*)` | identical |  |
| ivp_controller_phantom | `IVP_Controller_Phantom::IVP_Controller_Phantom(IVP_Real_Object*, IVP_Template_Phantom const*)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::do_steering(float, bool)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::do_steering_wheel(IVP_POS_WHEEL, float)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::CalcSteeringForces(IVP_Raycast_Car_Wheel_Temp*, IVP_Core*, IVP_Event_Sim*, float*)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::DoSimulationShocks(IVP_Raycast_Car_Wheel_Temp*, IVP_Ray_Hit*, IVP_Event_Sim*, IVP_Core*)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::DoSimulationWheels(IVP_Ray_Solver_Template*, IVP_U_Matrix const*, IVP_Raycast_Car_Wheel_Temp*, IVP_Ray_Hit*, float*, IVP_C` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::SetupWheelRaycasts(IVP_Ray_Solver_Template*, IVP_U_Matrix const*, IVP_Raycast_Car_Wheel_Temp*)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::ApplySteeringForces(IVP_Raycast_Car_Wheel_Temp*, IVP_Core*, IVP_Event_Sim*, float*)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::change_wheel_torque(IVP_POS_WHEEL, float)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::InitRaycastCarWheels(IVP_Template_Car_System const*)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::DoSimulationStabilizers(IVP_Raycast_Car_Wheel_Temp*)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::get_orig_axles_distance()` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::do_simulation_controller(IVP_Event_Sim*, IVP_U_Vector<IVP_Core>*)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::get_orig_front_wheel_distance()` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::IVP_Controller_Raycast_Car(IVP_Environment*, IVP_Template_Car_System const*)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::~IVP_Controller_Raycast_Car()` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::~IVP_Controller_Raycast_Car()` | identical |  |
| ivp_controller_raycast_car | `non-virtual thunk to IVP_Controller_Raycast_Car::~IVP_Controller_Raycast_Car()` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::get_skid_info(IVP_Wheel_Skid_Info*)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::get_body_speed(IVP_COORDINATE_INDEX)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::activate_booster(float, float, float)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::get_wheel_torque(IVP_POS_WHEEL)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::get_booster_delay()` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::DoSimulationBooster(IVP_Event_Sim*, IVP_Core*)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::change_spring_length(IVP_POS_WHEEL, float)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::GetCarSystemDebugData(IVP_CarSystemDebugData_t&)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::SetCarSystemDebugData(IVP_CarSystemDebugData_t const&)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::change_body_downforce(float)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::change_spring_constant(IVP_POS_WHEEL, float)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::change_fast_turn_factor(float)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::change_spring_dampening(IVP_POS_WHEEL, float)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::get_controller_priority()` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::set_booster_acceleration(float)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::change_spring_pre_tension(IVP_POS_WHEEL, float)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::change_stabilizer_constant(IVP_POS_AXIS, float)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::get_wheel_angular_velocity(IVP_POS_WHEEL)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::change_spring_dampening_compression(IVP_POS_WHEEL, float)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::fix_wheel(IVP_POS_WHEEL, IVP_BOOL)` | identical |  |
| ivp_controller_raycast_car | `IVP_Controller_Raycast_Car::~IVP_Controller_Raycast_Car()` | identical |  |
| ivp_controller_raycast_car | `non-virtual thunk to IVP_Controller_Raycast_Car::get_controller_priority()` | identical |  |
| ivp_controller_raycast_car | `non-virtual thunk to IVP_Controller_Raycast_Car::do_simulation_controller(IVP_Event_Sim*, IVP_U_Vector<IVP_Core>*)` | identical |  |
| ivp_controller_raycast_car | `non-virtual thunk to IVP_Controller_Raycast_Car::~IVP_Controller_Raycast_Car()` | identical |  |
| ivp_core | `IVP_Core::calc_movement_state(IVP_Time)` | differs | size retail 1029 vs ours 731 |
| ivp_core | `IVP_Core::revive_adjacent_to_unmoveable()` | differs | size retail 367 vs ours 335 |
| ivp_core | `IVP_Core::values_changed_recalc_redundants()` | differs | size retail 824 vs ours 778 |
| ivp_core | `IVP_Core::reset_time(IVP_Time)` | identical |  |
| ivp_core | `IVP_Core::damp_object(double, IVP_U_Float_Point const*, double)` | identical |  |
| ivp_core | `IVP_Core::push_core_ws(IVP_U_Point const*, IVP_U_Float_Point const*)` | identical |  |
| ivp_core | `IVP_Core::global_damp_core(double)` | identical |  |
| ivp_core | `IVP_Core::add_friction_info(IVP_Friction_Info_For_Core*)` | identical |  |
| ivp_core | `IVP_Core::get_friction_info(IVP_Friction_System*)` | identical |  |
| ivp_core | `IVP_Core::async_push_core_ws(IVP_U_Point const*, IVP_U_Float_Point const*)` | identical |  |
| ivp_core | `IVP_Core::get_energy_on_test(IVP_U_Float_Point const*, IVP_U_Float_Point const*)` | identical |  |
| ivp_core | `IVP_Core::apply_velocity_limit()` | edited |  |
| ivp_core | `IVP_Core::delete_friction_info(IVP_Friction_Info_For_Core*)` | identical |  |
| ivp_core | `IVP_Core::get_surface_speed_ws(IVP_U_Point const*, IVP_U_Float_Point*)` | identical |  |
| ivp_core | `IVP_Core::set_rotation_inertia(IVP_U_Float_Point const*)` | identical |  |
| ivp_core | `IVP_Core::unlink_friction_info(IVP_Friction_Info_For_Core*)` | identical |  |
| ivp_core | `IVP_Core::stop_physical_movement()` | identical |  |
| ivp_core | `IVP_Core::synchronize_with_rot_z()` | identical |  |
| ivp_core | `IVP_Core::init_core_for_simulation()` | identical |  |
| ivp_core | `IVP_Core::transform_PSI_matrizes_core(IVP_U_Matrix const*)` | identical |  |
| ivp_core | `IVP_Core::async_rot_push_core_multiple_ws(IVP_U_Float_Point const*, double)` | identical |  |
| ivp_core | `IVP_Core::ensure_core_to_be_in_simulation()` | identical |  |
| ivp_core | `IVP_Core::update_exact_mindist_events_of_core()` | identical |  |
| ivp_core | `IVP_Core::unlink_obj_from_core_and_maybe_destroy(IVP_Real_Object*)` | identical |  |
| ivp_core | `IVP_Core::init(IVP_Real_Object*, bool)` | identical |  |
| ivp_core | `IVP_Core::set_mass(float)` | identical |  |
| ivp_core | `IVP_Core::calc_calc()` | identical |  |
| ivp_core | `IVP_Core::IVP_Core(IVP_Real_Object*, IVP_U_Quat const*, IVP_U_Point const*, IVP_BOOL, IVP_BOOL)` | identical |  |
| ivp_core | `IVP_Core::~IVP_Core()` | identical |  |
| ivp_core | `IVP_Core::calc_at_matrix(IVP_Time, IVP_U_Matrix*) const` | identical |  |
| ivp_core | `IVP_Core::calc_virt_mass(IVP_U_Float_Point const*, IVP_U_Float_Point const*) const` | identical |  |
| ivp_core | `IVP_Core::calc_correct_virt_mass(IVP_U_Float_Point const*, IVP_U_Float_Point const*, IVP_U_Float_Point const*) const` | identical |  |
| ivp_core | `IVP_Core::calc_virt_mass_worst_case(IVP_U_Float_Point const*) const` | identical |  |
| ivp_core | `IVP_Core::set_radius(float, float)` | identical |  |
| ivp_core | `IVP_Core::async_push_core(IVP_U_Float_Point const*, IVP_U_Float_Point const*, IVP_U_Float_Point const*)` | identical |  |
| ivp_core | `IVP_Core::get_rot_speed_cs(IVP_U_Float_Point const*)` | identical |  |
| ivp_core | `IVP_Core::rot_push_core_cs(IVP_U_Float_Point const*)` | identical |  |
| ivp_core | `IVP_Core::abort_all_async_pushes()` | identical |  |
| ivp_core | `IVP_Core::undo_synchronize_rot_z()` | identical |  |
| ivp_core | `IVP_Core::commit_all_async_pushes()` | identical |  |
| ivp_core | `IVP_Core::reset_freeze_check_values()` | identical |  |
| ivp_core | `IVP_Core::rot_push_core_multiple_cs(IVP_U_Float_Point const*, double)` | identical |  |
| ivp_core | `IVP_Core::center_push_core_multiple_ws(IVP_U_Float_Point const*, double)` | identical |  |
| ivp_core | `IVP_Core::test_rot_push_core_multiple_cs(IVP_U_Float_Point const*, double, IVP_U_Float_Point*)` | identical |  |
| ivp_core | `IVP_Core::async_rot_push_core_multiple_cs(IVP_U_Float_Point const*, double)` | identical |  |
| ivp_core | `IVP_Core::calc_next_PSI_matrix_zero_speed(IVP_Event_Sim*)` | identical |  |
| ivp_core | `IVP_Core::moveable_core_has_friction_info()` | identical |  |
| ivp_core | `IVP_Core::get_diff_surface_speed_of_two_cores(IVP_Core const*, IVP_Core const*, IVP_U_Float_Point const*, IVP_U_Float_Point const*, IVP_U_Float_Point*` | identical |  |
| ivp_core | `IVP_Core::get_diff_surface_speed_of_two_cores_on_test(IVP_Core const*, IVP_Core const*, IVP_U_Float_Point const*, IVP_U_Float_Point const*, IVP_U_Floa` | identical |  |
| ivp_core | `IVP_Core::push_core(IVP_U_Float_Point const*, IVP_U_Float_Point const*, IVP_U_Float_Point const*)` | identical |  |
| ivp_core | `IVP_Core::IVP_Core(IVP_Real_Object*, IVP_U_Quat const*, IVP_U_Point const*, IVP_BOOL, IVP_BOOL)` | identical |  |
| ivp_core | `IVP_Core::test_push_core(IVP_U_Float_Point const*, IVP_U_Float_Point const*, IVP_U_Float_Point const*, IVP_U_Float_Point*, IVP_U_Float_Point*) const` | identical |  |
| ivp_core | `IVP_Core::get_surface_speed(IVP_U_Float_Point const*, IVP_U_Float_Point*) const` | identical |  |
| ivp_core | `IVP_Core::get_surface_speed_on_test(IVP_U_Float_Point const*, IVP_U_Float_Point const*, IVP_U_Float_Point const*, IVP_U_Float_Point*) const` | identical |  |
| ivp_environment | `_GLOBAL__I_a` | identical |  |
| ivp_environment | `IVP_Environment::reset_time()` | identical |  |
| ivp_environment | `IVP_Environment::create_ball(IVP_Template_Ball const*, IVP_Template_Real_Object const*, IVP_U_Quat const*, IVP_U_Point const*)` | identical |  |
| ivp_environment | `IVP_Environment::set_gravity(IVP_U_Point*)` | identical |  |
| ivp_environment | `IVP_Environment::create_force(IVP_Template_Force*)` | identical |  |
| ivp_environment | `IVP_Environment::simulate_psi(IVP_Time)` | identical |  |
| ivp_environment | `IVP_Environment::create_spring(IVP_Template_Spring*)` | identical |  |
| ivp_environment | `IVP_Environment::create_torque(IVP_Template_Torque*)` | identical |  |
| ivp_environment | `IVP_Environment::create_polygon(IVP_SurfaceManager*, IVP_Template_Real_Object const*, IVP_U_Quat const*, IVP_U_Point const*)` | identical |  |
| ivp_environment | `IVP_Environment::simulate_dtime(double)` | identical |  |
| ivp_environment | `IVP_Environment::create_stabilizer(IVP_Template_Stabilizer*)` | identical |  |
| ivp_environment | `IVP_Environment::create_suspension(IVP_Template_Suspension*)` | identical |  |
| ivp_environment | `IVP_Environment::set_delta_PSI_time(double)` | edited |  |
| ivp_environment | `IVP_Environment::simulate_time_step(float)` | identical |  |
| ivp_environment | `IVP_Environment::add_listener_object_global(IVP_Listener_Object*)` | identical |  |
| ivp_environment | `IVP_Environment::force_psi_on_next_simulation()` | edited |  |
| ivp_environment | `IVP_Environment::add_listener_collision_global(IVP_Listener_Collision*)` | identical |  |
| ivp_environment | `IVP_Environment::add_listener_constraint_global(IVP_Listener_Constraint*)` | identical |  |
| ivp_environment | `IVP_Environment::get_global_collision_tolerance()` | identical |  |
| ivp_environment | `IVP_Environment::set_global_collision_tolerance(double, double)` | identical |  |
| ivp_environment | `IVP_Environment::IVP_Environment(IVP_Environment_Manager*, IVP_Application_Environment*, char const*, unsigned int)` | edited |  |
| ivp_environment | `IVP_Environment::~IVP_Environment()` | identical |  |
| ivp_environment | `IVP_U_Set_Active<IVP_Real_Object>::~IVP_U_Set_Active()` | identical |  |
| ivp_environment | `IVP_U_Set_Active<IVP_Real_Object>::~IVP_U_Set_Active()` | identical |  |
| ivp_environment | `IVP_U_Set_Active<IVP_Mindist_Base>::~IVP_U_Set_Active()` | identical |  |
| ivp_environment | `IVP_U_Set_Active<IVP_Mindist_Base>::~IVP_U_Set_Active()` | identical |  |
| ivp_environment | `IVP_U_Set_Active<IVP_Core>::~IVP_U_Set_Active()` | identical |  |
| ivp_environment | `IVP_U_Set_Active<IVP_Core>::~IVP_U_Set_Active()` | identical |  |
| ivp_environment | `IVP_Attacher_To_Cores<IVP_Controller_Buoyancy>::IVP_Attacher_To_Cores(IVP_U_Set_Active<IVP_Core>*)` | identical |  |
| ivp_environment | `IVP_Environment_Manager::create_environment(IVP_Application_Environment*, char const*, unsigned int)` | identical |  |
| ivp_environment | `IVP_Environment_Manager::get_environment_manager()` | identical |  |
| ivp_environment | `IVP_Environment_Manager::~IVP_Environment_Manager()` | identical |  |
| ivp_environment | `IVP_U_Set<IVP_Real_Object>::~IVP_U_Set()` | identical |  |
| ivp_environment | `IVP_U_Set<IVP_Real_Object>::~IVP_U_Set()` | identical |  |
| ivp_environment | `IVP_U_Set<IVP_Mindist_Base>::~IVP_U_Set()` | identical |  |
| ivp_environment | `IVP_U_Set<IVP_Mindist_Base>::~IVP_U_Set()` | identical |  |
| ivp_environment | `IVP_U_Set<IVP_Core>::~IVP_U_Set()` | identical |  |
| ivp_environment | `IVP_U_Set<IVP_Core>::~IVP_U_Set()` | identical |  |
| ivp_environment | `IVP_Environment::set_current_time(IVP_Time)` | identical |  |
| ivp_environment | `IVP_Environment::fire_event_object_frozen(IVP_Event_Object*)` | identical |  |
| ivp_environment | `IVP_Environment::fire_event_pre_collision(IVP_Event_Collision*)` | identical |  |
| ivp_environment | `IVP_Environment::fire_event_object_created(IVP_Event_Object*)` | identical |  |
| ivp_environment | `IVP_Environment::fire_event_object_deleted(IVP_Event_Object*)` | identical |  |
| ivp_environment | `IVP_Environment::fire_event_object_revived(IVP_Event_Object*)` | identical |  |
| ivp_environment | `IVP_Environment::fire_event_post_collision(IVP_Event_Collision*)` | identical |  |
| ivp_environment | `IVP_Environment::fire_event_friction_created(IVP_Event_Friction*)` | identical |  |
| ivp_environment | `IVP_Environment::fire_event_friction_deleted(IVP_Event_Friction*)` | identical |  |
| ivp_environment | `IVP_Environment::fire_event_constraint_broken(IVP_Constraint*)` | identical |  |
| ivp_environment | `IVP_Environment::fire_event_constraint_broken(hk_Breakable_Constraint*)` | identical |  |
| ivp_environment | `IVP_Environment::remove_listener_object_global(IVP_Listener_Object*)` | identical |  |
| ivp_environment | `IVP_Environment::fire_event_friction_pair_created(IVP_Friction_Core_Pair*)` | identical |  |
| ivp_environment | `IVP_Environment::fire_event_friction_pair_deleted(IVP_Friction_Core_Pair*)` | identical |  |
| ivp_environment | `IVP_Environment::remove_listener_collision_global(IVP_Listener_Collision*)` | identical |  |
| ivp_environment | `IVP_Environment::remove_listener_constraint_global(IVP_Listener_Constraint*)` | identical |  |
| ivp_environment | `IVP_Environment::fire_object_is_removed_from_collision_detection(IVP_Real_Object*)` | identical |  |
| ivp_environment | `IVP_Environment::~IVP_Environment()` | identical |  |
| ivp_environment | `IVP_U_Set_Active<IVP_Real_Object>::~IVP_U_Set_Active()` | identical |  |
| ivp_environment | `IVP_U_Set_Active<IVP_Mindist_Base>::~IVP_U_Set_Active()` | identical |  |
| ivp_environment | `IVP_U_Set_Active<IVP_Core>::~IVP_U_Set_Active()` | identical |  |
| ivp_environment | `IVP_U_Set<IVP_Real_Object>::compare(void*, void*) const` | identical |  |
| ivp_environment | `IVP_U_Set<IVP_Mindist_Base>::compare(void*, void*) const` | identical |  |
| ivp_environment | `IVP_U_Set<IVP_Core>::compare(void*, void*) const` | identical |  |
| ivp_friction | `IVP_Contact_Point::friction_force_local_constraint_1d(IVP_Event_Sim const*)` | differs | bytes |
| ivp_friction | `IVP_Contact_Point::friction_force_local_constraint_2d(IVP_Event_Sim const*)` | differs | size retail 706 vs ours 730 |
| ivp_friction | `IVP_Contact_Point::friction_force_local_constraint_2d_wheel(IVP_Core*, IVP_Impact_Solver_Long_Term*, IVP_Event_Sim const*, float&)` | differs | size retail 2409 vs ours 2302 |
| ivp_friction | `IVP_Contact_Point::IVP_Contact_Point(IVP_Mindist*)` | differs | size retail 538 vs ours 430 |
| ivp_friction | `IVP_Friction_System::calc_friction_forces(IVP_Event_Sim const*)` | differs | size retail 539 vs ours 523 |
| ivp_friction | `IVP_Friction_System::split_friction_system(IVP_Core*)` | differs | data ref at +0x1a (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_i |
| ivp_friction | `IVP_Friction_System::do_simulation_controller(IVP_Event_Sim*, IVP_U_Vector<IVP_Core>*)` | differs | size retail 389 vs ours 405 |
| ivp_friction | `IVP_Synapse_Friction::is_same_as(IVP_Synapse_Real const*) const` | differs | data ref at +0x31 (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_i |
| ivp_friction | `IVP_Contact_Point::two_values_friction(IVP_U_Float_Point*)` | identical |  |
| ivp_friction | `IVP_Contact_Point::static_friction_single(IVP_Event_Sim const*, float, float)` | identical |  |
| ivp_friction | `IVP_Contact_Point::calc_virtual_mass_of_mindist()` | identical |  |
| ivp_friction | `IVP_Contact_Point::get_and_set_real_friction_len(IVP_U_Float_Point*)` | identical |  |
| ivp_friction | `IVP_Contact_Point::IVP_Contact_Point(IVP_Mindist*)` | identical |  |
| ivp_friction | `IVP_Contact_Point::~IVP_Contact_Point()` | identical |  |
| ivp_friction | `IVP_Friction_Solver::ease_two_mindists(IVP_Contact_Point*, IVP_Contact_Point*, IVP_U_Float_Point*, IVP_U_Float_Point*, double)` | identical |  |
| ivp_friction | `IVP_Friction_Solver::ease_friction_pair(IVP_Friction_Core_Pair*, IVP_U_Memory*)` | identical |  |
| ivp_friction | `IVP_Friction_Solver::IVP_Friction_Solver(IVP_Friction_System*, IVP_Event_Sim const*)` | identical |  |
| ivp_friction | `IVP_Friction_System::reset_time(IVP_Time)` | identical |  |
| ivp_friction | `IVP_Friction_System::add_fr_pair(IVP_Friction_Core_Pair*)` | identical |  |
| ivp_friction | `IVP_Friction_System::del_fr_pair(IVP_Friction_Core_Pair*)` | identical |  |
| ivp_friction | `IVP_Friction_System::fusion_friction_systems(IVP_Friction_System*)` | identical |  |
| ivp_friction | `IVP_Friction_System::delete_friction_distance(IVP_Contact_Point*)` | identical |  |
| ivp_friction | `IVP_Mutual_Energizer::init_mutual_energizer(IVP_Core*, IVP_Core*)` | identical |  |
| ivp_friction | `IVP_Mutual_Energizer::destroy_percent_energy(double)` | identical |  |
| ivp_friction | `IVP_Contact_Point_API::get_surface_normal_ws(IVP_Contact_Point*, IVP_U_Float_Point*)` | identical |  |
| ivp_friction | `IVP_Friction_Core_Pair::add_fr_dist_obj_pairs(IVP_Contact_Point*)` | identical |  |
| ivp_friction | `IVP_Friction_Core_Pair::destroy_mutual_energy(double)` | new |  |
| ivp_friction | `IVP_Friction_Core_Pair::check_all_fr_mindists_to_be_valid(IVP_Friction_System*)` | identical |  |
| ivp_friction | `IVP_Friction_Core_Pair::~IVP_Friction_Core_Pair()` | identical |  |
| ivp_friction | `IVP_Friction_Sys_Energy::get_controller_name()` | identical |  |
| ivp_friction | `IVP_Friction_Sys_Energy::do_simulation_controller(IVP_Event_Sim*, IVP_U_Vector<IVP_Core>*)` | edited |  |
| ivp_friction | `IVP_Friction_Sys_Energy::~IVP_Friction_Sys_Energy()` | identical |  |
| ivp_friction | `IVP_Friction_Sys_Energy::~IVP_Friction_Sys_Energy()` | identical |  |
| ivp_friction | `IVP_Friction_Sys_Static::get_controller_name()` | identical |  |
| ivp_friction | `IVP_Friction_Sys_Static::do_simulation_controller(IVP_Event_Sim*, IVP_U_Vector<IVP_Core>*)` | identical |  |
| ivp_friction | `IVP_Friction_Sys_Static::core_is_going_to_be_deleted_event(IVP_Core*)` | identical |  |
| ivp_friction | `IVP_Friction_Sys_Static::~IVP_Friction_Sys_Static()` | identical |  |
| ivp_friction | `IVP_Friction_Sys_Static::~IVP_Friction_Sys_Static()` | identical |  |
| ivp_friction | `IVP_Friction_Info_For_Core::friction_info_insert_friction_dist(IVP_Contact_Point*)` | identical |  |
| ivp_friction | `IVP_Core::grow_friction_system()` | identical |  |
| ivp_friction | `IVP_Contact_Point::set_friction_to_neutral()` | identical |  |
| ivp_friction | `IVP_Contact_Point::~IVP_Contact_Point()` | identical |  |
| ivp_friction | `IVP_Friction_Solver::IVP_Friction_Solver(IVP_Friction_System*, IVP_Event_Sim const*)` | identical |  |
| ivp_friction | `IVP_Friction_System::union_find_fr_sys()` | identical |  |
| ivp_friction | `IVP_Friction_System::find_pair_of_cores(IVP_Core*, IVP_Core*)` | identical |  |
| ivp_friction | `IVP_Friction_System::get_pair_info_for_objs(IVP_Core*, IVP_Core*)` | identical |  |
| ivp_friction | `IVP_Friction_System::get_associated_controlled_cores()` | identical |  |
| ivp_friction | `IVP_Friction_System::get_minimum_simulation_frequency()` | identical |  |
| ivp_friction | `IVP_Friction_System::core_is_going_to_be_deleted_event(IVP_Core*)` | identical |  |
| ivp_friction | `IVP_Friction_System::remove_energy_gained_by_real_friction()` | new |  |
| ivp_friction | `IVP_Contact_Point_API::get_vert_force(IVP_Contact_Point*)` | identical |  |
| ivp_friction | `IVP_Contact_Point_API::get_eliminated_energy(IVP_Contact_Point*)` | identical |  |
| ivp_friction | `IVP_Contact_Point_API::reset_eliminated_energy(IVP_Contact_Point*)` | identical |  |
| ivp_friction | `IVP_Friction_Core_Pair::number_of_pair_dists()` | identical |  |
| ivp_friction | `IVP_Friction_Core_Pair::del_fr_dist_obj_pairs(IVP_Contact_Point*)` | identical |  |
| ivp_friction | `IVP_Friction_Core_Pair::IVP_Friction_Core_Pair()` | identical |  |
| ivp_friction | `IVP_Friction_Sys_Energy::get_controller_priority()` | identical |  |
| ivp_friction | `IVP_Friction_Sys_Energy::core_is_going_to_be_deleted_event(IVP_Core*)` | identical |  |
| ivp_friction | `IVP_Friction_Sys_Static::get_controller_priority()` | identical |  |
| ivp_friction | `IVP_Friction_Sys_Static::get_minimum_simulation_frequency()` | identical |  |
| ivp_friction | `IVP_Core::union_find_get_father()` | identical |  |
| ivp_friction | `IVP_Contact_Point::is_same_as(IVP_Mindist const*) const` | edited |  |
| ivp_friction | `IVP_Synapse_Friction::get_material_index() const` | identical |  |
| ivp_friction_gaps | `IVP_Friction_Solver::calc_solver_PSI(IVP_Friction_System*, int*)` | differs | data ref at +0x512 (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_ |
| ivp_friction_gaps | `IVP_Friction_Solver::solve_linear_equation_and_push(IVP_Friction_System*, int*, int, IVP_U_Memory*)` | differs | size retail 783 vs ours 767 |
| ivp_friction_gaps | `IVP_Friction_Solver::async_apply_impulse(IVP_Impact_Solver_Long_Term const*, double)` | new |  |
| ivp_friction_gaps | `IVP_Friction_Solver::do_resulting_pushes(IVP_Friction_System*)` | edited |  |
| ivp_friction_gaps | `IVP_Friction_Solver::setup_coords_mindists(IVP_Friction_System*)` | identical |  |
| ivp_friction_gaps | `IVP_Friction_Solver::normize_constraint_equ()` | identical |  |
| ivp_friction_gaps | `IVP_Friction_Solver::test_gauss_solution_suggestion(double*, int*, int, IVP_U_Memory*)` | identical |  |
| ivp_friction_gaps | `IVP_Friction_System::do_friction_system(IVP_Event_Sim const*)` | edited |  |
| ivp_friction_gaps | `IVP_Friction_System::kinetic_energy_of_hole_frs()` | identical |  |
| ivp_friction_gaps | `IVP_Friction_System::reorder_mindists_for_complex()` | identical |  |
| ivp_friction_gaps | `IVP_Friction_System::bubble_sort_dists_importance()` | identical |  |
| ivp_great_matrix | `IVP_Incr_L_U_Matrix::decrement_l_u(int)` | identical |  |
| ivp_great_matrix | `IVP_Incr_L_U_Matrix::increment_l_u()` | identical |  |
| ivp_great_matrix | `IVP_Incr_L_U_Matrix::solve_lin_equ()` | identical |  |
| ivp_great_matrix | `IVP_Incr_L_U_Matrix::pivot_search_l_u(int)` | identical |  |
| ivp_great_matrix | `IVP_Incr_L_U_Matrix::add_neg_row_to_row_l_u(int, int, double)` | identical |  |
| ivp_great_matrix | `IVP_Incr_L_U_Matrix::l_u_decomposition_with_pivoting()` | identical |  |
| ivp_great_matrix | `IVP_Great_Matrix_Many_Zero::solve_lower_null_matrix()` | identical |  |
| ivp_great_matrix | `IVP_Great_Matrix_Many_Zero::matrix_check_unequation_line(int)` | identical |  |
| ivp_great_matrix | `IVP_Great_Matrix_Many_Zero::transform_to_lower_null_triangle()` | identical |  |
| ivp_great_matrix | `IVP_Great_Matrix_Many_Zero::invert(IVP_Great_Matrix_Many_Zero*)` | identical |  |
| ivp_great_matrix | `IVP_Great_Matrix_Many_Zero::lu_crout(int*, double*)` | identical |  |
| ivp_great_matrix | `IVP_Great_Matrix_Many_Zero::lu_solve(int*)` | identical |  |
| ivp_great_matrix | `IVP_Linear_Constraint_Solver::full_setup()` | identical |  |
| ivp_great_matrix | `IVP_Linear_Constraint_Solver::alloc_memory(IVP_U_Memory*)` | identical |  |
| ivp_great_matrix | `IVP_Linear_Constraint_Solver::startup_setup(int)` | identical |  |
| ivp_great_matrix | `IVP_Linear_Constraint_Solver::get_fdirection()` | identical |  |
| ivp_great_matrix | `IVP_Linear_Constraint_Solver::init_and_solve_lc(double*, double*, double*, int, int, IVP_U_Memory*)` | identical |  |
| ivp_great_matrix | `IVP_Linear_Constraint_Solver::full_setup_test_ranges()` | identical |  |
| ivp_great_matrix | `IVP_Linear_Constraint_Solver::numerical_stability_ok()` | identical |  |
| ivp_great_matrix | `IVP_Linear_Constraint_Solver::solve_lc()` | identical |  |
| ivp_great_matrix | `IVP_Great_Matrix_Many_Zero::align_matrix_values()` | identical |  |
| ivp_great_matrix | `IVP_Great_Matrix_Many_Zero::fill_from_bigger_matrix(IVP_Great_Matrix_Many_Zero*, int*, int)` | identical |  |
| ivp_great_matrix | `IVP_Great_Matrix_Many_Zero::solve_great_matrix_many_zero()` | identical |  |
| ivp_great_matrix | `IVP_Great_Matrix_Many_Zero::mult()` | identical |  |
| ivp_great_matrix | `IVP_Great_Matrix_Many_Zero::set_value(double, int, int)` | identical |  |
| ivp_great_matrix | `IVP_Great_Matrix_Many_Zero::IVP_Great_Matrix_Many_Zero()` | identical |  |
| ivp_great_matrix | `IVP_Linear_Constraint_Solver::update_step_vars(double)` | identical |  |
| ivp_great_matrix | `IVP_Linear_Constraint_Solver::increment_sub_solver()` | identical |  |
| ivp_great_matrix | `IVP_Linear_Constraint_Solver::get_values_when_setup()` | identical |  |
| ivp_great_matrix | `IVP_Linear_Constraint_Solver::lcs_bubble_sort_x_vals()` | identical |  |
| ivp_great_matrix | `IVP_Linear_Constraint_Solver::do_a_little_random_permutation()` | identical |  |
| ivp_great_matrix | `IVP_Linear_Constraint_Solver::move_not_necessary_actives_to_inactives()` | identical |  |
| ivp_halfspacesoup | `IVP_Halfspacesoup::add_halfspace(IVP_U_Hesse const*)` | identical |  |
| ivp_halfspacesoup | `IVP_Halfspacesoup::~IVP_Halfspacesoup()` | identical |  |
| ivp_halfspacesoup | `IVP_Halfspacesoup::IVP_Halfspacesoup()` | new |  |
| ivp_halfspacesoup | `IVP_Halfspacesoup::~IVP_Halfspacesoup()` | identical |  |
| ivp_hull_manager | `IVP_Hull_Manager::reset_times()` | edited |  |
| ivp_hull_manager | `IVP_Hull_Manager_Base::IVP_Hull_Manager_Base()` | identical |  |
| ivp_hull_manager | `IVP_Hull_Manager_Base::~IVP_Hull_Manager_Base()` | identical |  |
| ivp_hull_manager | `IVP_Hull_Manager::reset_time(IVP_Time)` | identical |  |
| ivp_hull_manager | `IVP_Hull_Manager::delete_hull_manager()` | identical |  |
| ivp_hull_manager | `IVP_Hull_Manager::get_synapse_check_count(IVP_Environment*)` | new |  |
| ivp_hull_manager | `IVP_Hull_Manager::too_many_synapses_checked_error_get_additional(IVP_Environment*, int)` | new |  |
| ivp_hull_manager | `IVP_Listener_Hull::hull_manager_is_reset(float, float)` | identical |  |
| ivp_i_collision_vhash | `IVP_Collision_Callback_Table_Hash::object_to_index(IVP_Real_Object*)` | identical |  |
| ivp_i_collision_vhash | `IVP_Collision_Callback_Table_Hash::~IVP_Collision_Callback_Table_Hash()` | identical |  |
| ivp_i_collision_vhash | `IVP_Collision_Callback_Table_Hash::~IVP_Collision_Callback_Table_Hash()` | identical |  |
| ivp_i_collision_vhash | `IVP_Collision_Callback_Table_Hash::compare(void*, void*) const` | identical |  |
| ivp_i_fpoint_vhash | `IVP_I_FPoint_VHash::point_to_index(IVP_U_Float_Point*)` | identical |  |
| ivp_i_fpoint_vhash | `IVP_I_FPoint_VHash::~IVP_I_FPoint_VHash()` | identical |  |
| ivp_i_fpoint_vhash | `IVP_I_FPoint_VHash::~IVP_I_FPoint_VHash()` | identical |  |
| ivp_i_fpoint_vhash | `IVP_I_FPoint_VHash::compare(void*, void*) const` | identical |  |
| ivp_i_friction_hash | `IVP_Friction_Hash::~IVP_Friction_Hash()` | identical |  |
| ivp_i_object_vhash | `IVP_Object_Callback_Table_Hash::object_to_index(IVP_Real_Object*)` | identical |  |
| ivp_i_object_vhash | `IVP_Object_Callback_Table_Hash::~IVP_Object_Callback_Table_Hash()` | identical |  |
| ivp_i_object_vhash | `IVP_Object_Callback_Table_Hash::~IVP_Object_Callback_Table_Hash()` | identical |  |
| ivp_i_object_vhash | `IVP_Object_Callback_Table_Hash::compare(void*, void*) const` | identical |  |
| ivp_i_point_vhash | `IVP_point_hash::point_to_index(IVP_Poly_Point*)` | identical |  |
| ivp_i_point_vhash | `IVP_point_hash::~IVP_point_hash()` | identical |  |
| ivp_i_point_vhash | `IVP_point_hash::~IVP_point_hash()` | identical |  |
| ivp_i_point_vhash | `IVP_I_Point_VHash::point_to_index(IVP_U_Point*)` | identical |  |
| ivp_i_point_vhash | `IVP_I_Point_VHash::~IVP_I_Point_VHash()` | identical |  |
| ivp_i_point_vhash | `IVP_I_Point_VHash::~IVP_I_Point_VHash()` | identical |  |
| ivp_i_point_vhash | `IVP_point_hash::compare(void*, void*) const` | identical |  |
| ivp_i_point_vhash | `IVP_I_Point_VHash::compare(void*, void*) const` | identical |  |
| ivp_impact | `IVP_Mindist::try_to_generate_managed_friction(IVP_Friction_System**, IVP_BOOL*, IVP_Simulation_Unit*, IVP_BOOL)` | differs | size retail 1017 vs ours 1005 |
| ivp_impact | `IVP_Impact_Solver::delay_decision(IVP_Core**)` | differs | size retail 1259 vs ours 1270 |
| ivp_impact | `IVP_Impact_Solver::do_rescue_push(IVP_U_Float_Point*, IVP_BOOL)` | differs | bytes |
| ivp_impact | `IVP_Impact_System::test_loop_all_pairs()` | differs | size retail 771 vs ours 739 |
| ivp_impact | `IVP_Impact_System::recalc_all_affected_cores()` | differs | bytes |
| ivp_impact | `IVP_Impact_Solver_Long_Term::do_impact_of_two_objects(IVP_Mindist*, IVP_Real_Object*, IVP_Real_Object*)` | differs | size retail 1043 vs ours 1112 |
| ivp_impact | `IVP_Mindist::do_impact()` | identical |  |
| ivp_impact | `IVP_Contact_Point::get_material_info(IVP_Material**)` | identical |  |
| ivp_impact | `IVP_Contact_Point::calc_coll_distance()` | identical |  |
| ivp_impact | `IVP_Contact_Point::get_rot_speed_uncertainty()` | edited |  |
| ivp_impact | `IVP_Contact_Point::read_materials_for_contact_situation(IVP_Impact_Solver_Long_Term*)` | identical |  |
| ivp_impact | `IVP_Impact_Solver::get_world_push_direction()` | identical |  |
| ivp_impact | `IVP_Impact_Solver::calc_virt_masses_impact_solver(IVP_U_Float_Point const*)` | identical |  |
| ivp_impact | `IVP_Impact_Solver::get_world_direction_second_friction(IVP_Contact_Point*)` | identical |  |
| ivp_impact | `IVP_Impact_Solver::get_world_push_direction_two_friction(double)` | identical |  |
| ivp_impact | `IVP_Impact_Solver::do_push(double)` | identical |  |
| ivp_impact | `IVP_Impact_Solver::do_impact(IVP_Core**, IVP_BOOL, int, float)` | identical |  |
| ivp_impact | `IVP_Impact_System::init_and_solve_impact_system(IVP_Mindist*, IVP_Friction_System*, IVP_Friction_Core_Pair*, IVP_Contact_Point*)` | identical |  |
| ivp_impact | `IVP_Impact_System::add_pushed_core_with_pairs_except(IVP_Core*, IVP_Friction_Core_Pair*)` | identical |  |
| ivp_impact | `IVP_Impact_Solver_Long_Term::do_impact_long_term(IVP_Core**, float, IVP_Contact_Point*)` | identical |  |
| ivp_impact | `IVP_Impact_Solver::undo_push()` | identical |  |
| ivp_material | `IVP_Material::~IVP_Material()` | identical |  |
| ivp_material | `IVP_Material_Simple::get_name()` | identical |  |
| ivp_material | `IVP_Material_Simple::~IVP_Material_Simple()` | identical |  |
| ivp_material | `IVP_Material_Manager::get_material_by_index(IVP_Real_Object*, IVP_U_Point const*, int)` | identical |  |
| ivp_material | `IVP_Material_Manager::IVP_Material_Manager(IVP_BOOL)` | identical |  |
| ivp_material | `IVP_Material_Manager::IVP_Material_Manager(IVP_BOOL)` | identical |  |
| ivp_material | `IVP_Material_Manager::~IVP_Material_Manager()` | identical |  |
| ivp_material | `IVP_Material_Manager::~IVP_Material_Manager()` | identical |  |
| ivp_material | `IVP_Material::~IVP_Material()` | identical |  |
| ivp_material | `IVP_Material::~IVP_Material()` | identical |  |
| ivp_material | `IVP_Material_Simple::get_damping()` | identical |  |
| ivp_material | `IVP_Material_Simple::get_adhesion()` | identical |  |
| ivp_material | `IVP_Material_Simple::get_elasticity()` | identical |  |
| ivp_material | `IVP_Material_Simple::get_friction_factor()` | identical |  |
| ivp_material | `IVP_Material_Simple::get_second_friction_factor()` | identical |  |
| ivp_material | `IVP_Material_Simple::~IVP_Material_Simple()` | identical |  |
| ivp_material | `IVP_Material_Manager::get_damping(IVP_Contact_Situation*)` | identical |  |
| ivp_material | `IVP_Material_Manager::get_adhesion(IVP_Contact_Situation*)` | identical |  |
| ivp_material | `IVP_Material_Manager::get_elasticity(IVP_Contact_Situation*)` | identical |  |
| ivp_material | `IVP_Material_Manager::get_friction_factor(IVP_Contact_Situation*)` | identical |  |
| ivp_mindist | `IVP_Mindist::init_mindist(IVP_Real_Object*, IVP_Real_Object*, IVP_Compact_Edge const*, IVP_Compact_Edge const*)` | differs | size retail 496 vs ours 506 |
| ivp_mindist | `IVP_Time_Event::simulate_time_event(IVP_Environment*)` | differs | data ref at +0xe (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_ph |
| ivp_mindist | `_GLOBAL__I_a` | identical |  |
| ivp_mindist | `IVP_Mindist::simulate_time_event(IVP_Environment*)` | identical |  |
| ivp_mindist | `IVP_Mindist::exact_mindist_went_invalid(IVP_Mindist_Manager*)` | identical |  |
| ivp_mindist | `IVP_Mindist::update_exact_mindist_events(IVP_BOOL, IVP_MINDIST_EVENT_HINT)` | identical |  |
| ivp_mindist | `IVP_Mindist::create_cp_in_advance_pretension(IVP_Real_Object*, float)` | edited |  |
| ivp_mindist | `IVP_Mindist::mindist_hull_limit_exceeded_event(float)` | identical |  |
| ivp_mindist | `IVP_Mindist::IVP_Mindist(IVP_Environment*, IVP_Collision_Delegator*)` | identical |  |
| ivp_mindist | `IVP_Mindist::IVP_Mindist(IVP_Environment*, IVP_Collision_Delegator*)` | identical |  |
| ivp_mindist | `IVP_Mindist::~IVP_Mindist()` | identical |  |
| ivp_mindist | `IVP_Mindist::~IVP_Mindist()` | identical |  |
| ivp_mindist | `IVP_Synapse::~IVP_Synapse()` | identical |  |
| ivp_mindist | `IVP_Synapse::~IVP_Synapse()` | identical |  |
| ivp_mindist | `IVP_Collision::~IVP_Collision()` | identical |  |
| ivp_mindist | `IVP_Collision::~IVP_Collision()` | identical |  |
| ivp_mindist | `IVP_Diff_Hash<IVP_MM_CMP, IVP_U_FVector<IVP_Collision>, IVP_Collision, IVP_MM_CMP_Key>::check_element(IVP_MM_CMP_Key*, IVP_MM_CMP_Key*)` | identical |  |
| ivp_mindist | `IVP_Diff_Hash<IVP_MM_CMP, IVP_U_FVector<IVP_Collision>, IVP_Collision, IVP_MM_CMP_Key>::IVP_Diff_Hash(IVP_U_FVector<IVP_Collision>*, short*, int, IVP_` | edited |  |
| ivp_mindist | `IVP_Diff_Hash<IVP_OO_CMP, IVP_U_FVector<IVP_Collision>, IVP_Collision, IVP_Real_Object>::check_element(IVP_Real_Object*, IVP_Real_Object*)` | identical |  |
| ivp_mindist | `IVP_Diff_Hash<IVP_OO_CMP, IVP_U_FVector<IVP_Collision>, IVP_Collision, IVP_Real_Object>::IVP_Diff_Hash(IVP_U_FVector<IVP_Collision>*, short*, int, IVP` | edited |  |
| ivp_mindist | `IVP_Mindist_Manager::recheck_ov_element(IVP_Real_Object*)` | identical |  |
| ivp_mindist | `IVP_Mindist_Manager::insert_hull_mindist(IVP_Mindist*, float)` | identical |  |
| ivp_mindist | `IVP_Mindist_Manager::remove_hull_mindist(IVP_Mindist*)` | identical |  |
| ivp_mindist | `IVP_Mindist_Manager::insert_exact_mindist(IVP_Mindist*)` | identical |  |
| ivp_mindist | `IVP_Mindist_Manager::recalc_exact_mindist(IVP_Mindist*)` | identical |  |
| ivp_mindist | `IVP_Mindist_Manager::remove_exact_mindist(IVP_Mindist*)` | identical |  |
| ivp_mindist | `IVP_Mindist_Manager::create_exact_mindists(IVP_Real_Object*, IVP_Real_Object*, double, IVP_U_FVector<IVP_Collision>*, IVP_Compact_Ledge const*, IVP_Co` | identical |  |
| ivp_mindist | `IVP_Mindist_Manager::insert_lazy_hull_mindist(IVP_Mindist*, float)` | identical |  |
| ivp_mindist | `IVP_Mindist_Manager::recalc_all_exact_wheel_mindist()` | identical |  |
| ivp_mindist | `IVP_Mindist_Manager::insert_and_recalc_exact_mindist(IVP_Mindist*)` | identical |  |
| ivp_mindist | `IVP_Mindist_Manager::insert_and_recalc_phantom_mindist(IVP_Mindist*)` | identical |  |
| ivp_mindist | `IVP_Mindist_Manager::enable_collision_detection_for_object(IVP_Real_Object*)` | identical |  |
| ivp_mindist | `IVP_Mindist_Manager::~IVP_Mindist_Manager()` | identical |  |
| ivp_mindist | `IVP_Mindist_Settings::set_collision_tolerance(double, double)` | identical |  |
| ivp_mindist | `IVP_Mindist_Settings::IVP_Mindist_Settings()` | identical |  |
| ivp_mindist | `IVP_Mindist::~IVP_Mindist()` | identical |  |
| ivp_mindist | `IVP_Synapse::hull_manager_is_reset(float, float)` | identical |  |
| ivp_mindist | `IVP_Synapse::hull_limit_exceeded_event(IVP_Hull_Manager*, float)` | identical |  |
| ivp_mindist | `IVP_Synapse::hull_manager_is_going_to_be_deleted_event(IVP_Hull_Manager*)` | identical |  |
| ivp_mindist | `IVP_Synapse::get_type()` | identical |  |
| ivp_mindist | `IVP_Mindist_Base::get_ledges(IVP_Compact_Ledge const**)` | identical |  |
| ivp_mindist | `IVP_Mindist_Base::get_objects(IVP_Real_Object**)` | identical |  |
| ivp_mindist | `IVP_Mindist_Manager::insert_invalid_mindist(IVP_Mindist*)` | identical |  |
| ivp_mindist | `IVP_Mindist_Manager::remove_invalid_mindist(IVP_Mindist*)` | identical |  |
| ivp_mindist | `IVP_Mindist_Manager::recalc_all_exact_mindists()` | identical |  |
| ivp_mindist | `IVP_Mindist_Manager::recalc_all_exact_mindists_events()` | identical |  |
| ivp_mindist | `IVP_Mindist_Manager::IVP_Mindist_Manager(IVP_Environment*)` | identical |  |
| ivp_mindist | `IVP_Synapse::get_ledge() const` | identical |  |
| ivp_mindist_event | `IVP_3D_Solver_KK_COLL::get_value(IVP_U_Matrix*, IVP_U_Matrix*)` | differs | size retail 348 vs ours 346 |
| ivp_mindist_event | `IVP_Mindist_Event_Solver::next_event_B_POLY(IVP_Mindist_Event_Solver*)` | differs | data ref at +0x18a (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_ |
| ivp_mindist_event | `IVP_Mindist_Event_Solver::calc_next_event_BP(IVP_Ball*, IVP_Compact_Edge const*, IVP_Cache_Object*, IVP_Cache_Ledge_Point*)` | differs | size retail 1986 vs ours 1992 |
| ivp_mindist_event | `IVP_Mindist_Event_Solver::calc_next_event_PF(IVP_Compact_Edge const*, IVP_Compact_Edge const*, IVP_Cache_Ledge_Point*, IVP_Cache_Ledge_Point*)` | differs | size retail 1960 vs ours 1981 |
| ivp_mindist_event | `IVP_Mindist_Event_Solver::calc_next_event_PP(IVP_Compact_Edge const*, IVP_Compact_Edge const*, IVP_Cache_Ledge_Point*, IVP_Cache_Ledge_Point*)` | differs | size retail 2251 vs ours 2238 |
| ivp_mindist_event | `IVP_Mindist_Event_Solver::next_event_illegal(IVP_Mindist_Event_Solver*)` | differs | data ref at +0xe (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_co |
| ivp_mindist_event | `IVP_Mindist_Event_Solver::next_event_default_poly_poly(IVP_Mindist_Event_Solver*)` | differs | data ref at +0x1f9 (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_ |
| ivp_mindist_event | `IVP_3D_Solver_PK_KK::get_value(IVP_U_Matrix*, IVP_U_Matrix*)` | identical |  |
| ivp_mindist_event | `IVP_3D_Solver_PF_NPF::get_value(IVP_U_Matrix*, IVP_U_Matrix*)` | identical |  |
| ivp_mindist_event | `IVP_3D_Solver_S_VALS::get_value(IVP_U_Matrix*, IVP_U_Matrix*)` | identical |  |
| ivp_mindist_event | `IVP_3D_Solver_PF_COLL::get_value(IVP_U_Matrix*, IVP_U_Matrix*)` | identical |  |
| ivp_mindist_event | `IVP_3D_Solver_PK_COLL::get_value(IVP_U_Matrix*, IVP_U_Matrix*)` | identical |  |
| ivp_mindist_event | `IVP_Mindist_Event_Solver::next_event_BB(IVP_Mindist_Event_Solver*)` | identical |  |
| ivp_mindist_event | `IVP_Mindist_Event_Solver::calc_next_event_BB(IVP_Cache_Object*, IVP_Cache_Object*)` | identical |  |
| ivp_mindist_event | `IVP_Mindist_Event_Solver::calc_next_event_BF(IVP_Compact_Edge const*, IVP_Cache_Object*, IVP_Cache_Ledge_Point*)` | identical |  |
| ivp_mindist_event | `IVP_Mindist_Event_Solver::calc_next_event_BK(IVP_Ball*, IVP_Compact_Edge const*, IVP_Cache_Object*, IVP_Cache_Ledge_Point*)` | identical |  |
| ivp_mindist_event | `IVP_Mindist_Event_Solver::calc_next_event_KK(IVP_Compact_Edge const*, IVP_Compact_Edge const*, IVP_Cache_Ledge_Point*, IVP_Cache_Ledge_Point*)` | identical |  |
| ivp_mindist_event | `IVP_Mindist_Event_Solver::calc_next_event_PK(IVP_Compact_Edge const*, IVP_Compact_Edge const*, IVP_Cache_Ledge_Point*, IVP_Cache_Ledge_Point*)` | identical |  |
| ivp_mindist_event | `IVP_Mindist_Event_Solver::init_mim_function_table()` | identical |  |
| ivp_mindist_event | `IVP_3D_Solver_KK_PARALLEL::get_value(IVP_U_Matrix*, IVP_U_Matrix*)` | identical |  |
| ivp_mindist_event | `IVP_3D_Solver_VEC_PARALLEL_AREA::get_value(IVP_U_Matrix*, IVP_U_Matrix*)` | identical |  |
| ivp_mindist_event | `IVP_3D_Solver_DISTANCE_OF_TWO_POINTS::get_value(IVP_U_Matrix*, IVP_U_Matrix*)` | identical |  |
| ivp_mindist_friction | `IVP_Contact_Point::p_calc_friction_s_PK(IVP_U_Point const*, IVP_Compact_Edge const*, IVP_Cache_Ledge_Point*, IVP_Impact_Solver_Long_Term*, IVP_U_Float` | differs | size retail 811 vs ours 852 |
| ivp_mindist_friction | `IVP_Contact_Point::p_calc_friction_ss_KK(IVP_Compact_Edge const*, IVP_Compact_Edge const*, IVP_Cache_Ledge_Point*, IVP_Cache_Ledge_Point*, IVP_Impact_` | differs | size retail 1659 vs ours 1481 |
| ivp_mindist_friction | `IVP_Contact_Point::recalc_friction_s_vals()` | differs | data ref at +0x2ba (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_ |
| ivp_mindist_friction | `IVP_Contact_Point::recompute_friction()` | new |  |
| ivp_mindist_friction | `IVP_Contact_Point::p_calc_friction_qr_PF(IVP_U_Point const*, IVP_Compact_Edge const*, IVP_Cache_Ledge_Point*, IVP_Impact_Solver_Long_Term*, IVP_U_Floa` | identical |  |
| ivp_mindist_friction | `IVP_Friction_Manager::generate_contact_point(IVP_Mindist*, IVP_BOOL*)` | identical |  |
| ivp_mindist_friction | `IVP_Contact_Point::reset_time(IVP_Time)` | identical |  |
| ivp_mindist_friction | `IVP_Contact_Point::get_contact_normal(IVP_U_Float_Point*)` | identical |  |
| ivp_mindist_mcases | `IVP_Mindist_Minimize_Solver::p_minimize_BP(IVP_Cache_Ball*, IVP_Compact_Edge const*, IVP_Cache_Ledge_Point*)` | differs | size retail 944 vs ours 997 |
| ivp_mindist_mcases | `IVP_Mindist_Minimize_Solver::p_minimize_PP(IVP_Compact_Edge const*, IVP_Compact_Edge const*, IVP_Cache_Ledge_Point*, IVP_Cache_Ledge_Point*)` | differs | size retail 1466 vs ours 1474 |
| ivp_mindist_mcases | `IVP_Mindist_Minimize_Solver::p_minimize_Leave_BK(IVP_Cache_Ball*, IVP_Compact_Edge const*, IVP_Cache_Ledge_Point*)` | differs | size retail 1323 vs ours 1355 |
| ivp_mindist_mcases | `IVP_Mindist_Minimize_Solver::p_minimize_Leave_KK(IVP_Compact_Edge const*, IVP_Compact_Edge const*, IVP_KK_Input const&, IVP_Unscaled_KK_Result const*,` | differs | size retail 2810 vs ours 2845 |
| ivp_mindist_mcases | `IVP_Mindist_Minimize_Solver::p_minimize_Leave_PF(IVP_Compact_Edge const*, IVP_U_Point const*, IVP_Compact_Edge const*, IVP_Cache_Ledge_Point*, IVP_Cac` | differs | size retail 1670 vs ours 1630 |
| ivp_mindist_mcases | `IVP_Mindist_Minimize_Solver::p_minimize_Leave_PK(IVP_Compact_Edge const*, IVP_Compact_Edge const*, IVP_Cache_Ledge_Point*, IVP_Cache_Ledge_Point*)` | differs | size retail 2117 vs ours 2140 |
| ivp_mindist_mcases | `IVP_Mindist_Minimize_Solver::p_minimize_BF(IVP_Cache_Ball*, IVP_Compact_Edge const*, IVP_Cache_Ledge_Point*)` | identical |  |
| ivp_mindist_mcases | `IVP_Mindist_Minimize_Solver::p_minimize_BK(IVP_Cache_Ball*, IVP_Compact_Edge const*, IVP_Cache_Ledge_Point*)` | identical |  |
| ivp_mindist_mcases | `IVP_Mindist_Minimize_Solver::p_minimize_KK(IVP_Compact_Edge const*, IVP_Compact_Edge const*, IVP_Cache_Ledge_Point*, IVP_Cache_Ledge_Point*)` | identical |  |
| ivp_mindist_mcases | `IVP_Mindist_Minimize_Solver::p_minimize_PF(IVP_Compact_Edge const*, IVP_Compact_Edge const*, IVP_Cache_Ledge_Point*, IVP_Cache_Ledge_Point*)` | identical |  |
| ivp_mindist_mcases | `IVP_Mindist_Minimize_Solver::p_minimize_PK(IVP_Compact_Edge const*, IVP_Compact_Edge const*, IVP_Cache_Ledge_Point*, IVP_Cache_Ledge_Point*)` | identical |  |
| ivp_mindist_minimize | `IVP_Mindist::recalc_mindist()` | differs | bytes |
| ivp_mindist_minimize | `IVP_Mindist::recalc_invalid_mindist()` | differs | data ref at +0x50 (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_c |
| ivp_mindist_minimize | `IVP_Mindist_Minimize_Solver::minimize_FB(IVP_Mindist_Minimize_Solver*)` | differs | data ref at +0x12 (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_c |
| ivp_mindist_minimize | `IVP_Mindist_Minimize_Solver::minimize_KB(IVP_Mindist_Minimize_Solver*)` | differs | data ref at +0x12 (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_c |
| ivp_mindist_minimize | `IVP_Mindist_Minimize_Solver::minimize_PB(IVP_Mindist_Minimize_Solver*)` | differs | data ref at +0x12 (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_c |
| ivp_mindist_minimize | `IVP_Mindist_Minimize_Solver::p_minimize_FF(IVP_Compact_Edge const*, IVP_Compact_Edge const*, IVP_Cache_Ledge_Point*, IVP_Cache_Ledge_Point*)` | differs | size retail 4856 vs ours 3152 |
| ivp_mindist_minimize | `IVP_Mindist_Minimize_Solver::minimize_B_POLY(IVP_Mindist_Minimize_Solver*)` | differs | data ref at +0x17f (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_ |
| ivp_mindist_minimize | `IVP_Mindist_Minimize_Solver::minimize_illegal(IVP_Mindist_Minimize_Solver*)` | differs | data ref at +0xe (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_co |
| ivp_mindist_minimize | `CFmtStrN<256, false>::InitQuietTruncation()` | missing |  |
| ivp_mindist_minimize | `CFmtStrN<256, false>::sprintf(char const*, ...)` | missing |  |
| ivp_mindist_minimize | `IVP_Compact_Ledge_Solver::minimize_on_other_side(IVP_Compact_Edge const*, IVP_U_Point const*)` | identical |  |
| ivp_mindist_minimize | `IVP_Mindist_Minimize_Solver::minimize_BB(IVP_Mindist_Minimize_Solver*)` | edited |  |
| ivp_mindist_minimize | `IVP_Mindist_Minimize_Solver::init_mms_function_table()` | identical |  |
| ivp_mindist_minimize | `IVP_Mindist_Minimize_Solver::minimize_default_poly_poly(IVP_Mindist_Minimize_Solver*)` | identical |  |
| ivp_mindist_minimize | `IVP_Mindist::is_recursive()` | identical |  |
| ivp_mindist_minimize | `IVP_Mindist::mindist_rescue_push()` | edited |  |
| ivp_mindist_minimize | `IVP_Collision::delegator_is_going_to_be_deleted_event(IVP_Collision_Delegator*)` | identical |  |
| ivp_mindist_minimize | `IVP_Mindist_Minimize_Solver::check_loop_hash(IVP_SYNAPSE_POLYGON_STATUS, IVP_Compact_Edge const*, IVP_SYNAPSE_POLYGON_STATUS, IVP_Compact_Edge const*)` | edited |  |
| ivp_mindist_minimize | `IVP_Mindist_Minimize_Solver::minimize_swapped_poly_poly(IVP_Mindist_Minimize_Solver*)` | identical |  |
| ivp_mindist_recursive | `IVP_Mindist_Recursive::exact_mindist_went_invalid(IVP_Mindist_Manager*)` | differs | data ref at +0x93 (constant): retail bytes a95f6358, ours bytes ec78ad60 |
| ivp_mindist_recursive | `IVP_Mindist_Recursive::do_impact()` | differs | data ref at +0x3d (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_c |
| ivp_mindist_recursive | `IVP_Mindist_Recursive::recheck_recursive_childs(double)` | identical |  |
| ivp_mindist_recursive | `IVP_Mindist_Recursive::invalid_mindist_went_exact()` | identical |  |
| ivp_mindist_recursive | `IVP_Mindist_Recursive::rec_hull_limit_exceeded_event()` | identical |  |
| ivp_mindist_recursive | `IVP_Mindist_Recursive::IVP_Mindist_Recursive(IVP_Environment*, IVP_Collision_Delegator*)` | identical |  |
| ivp_mindist_recursive | `IVP_Mindist_Recursive::~IVP_Mindist_Recursive()` | identical |  |
| ivp_mindist_recursive | `IVP_Mindist_Recursive::~IVP_Mindist_Recursive()` | identical |  |
| ivp_mindist_recursive | `non-virtual thunk to IVP_Mindist_Recursive::~IVP_Mindist_Recursive()` | identical |  |
| ivp_mindist_recursive | `IVP_Mindist_Recursive::is_recursive()` | identical |  |
| ivp_mindist_recursive | `IVP_Mindist_Recursive::mindist_rescue_push()` | identical |  |
| ivp_mindist_recursive | `IVP_Mindist_Recursive::get_spawned_mindist_count()` | identical |  |
| ivp_mindist_recursive | `IVP_Mindist_Recursive::change_spawned_mindist_count(int)` | identical |  |
| ivp_mindist_recursive | `IVP_Mindist_Recursive::collision_is_going_to_be_deleted_event(IVP_Collision*)` | identical |  |
| ivp_mindist_recursive | `IVP_Mindist_Recursive::~IVP_Mindist_Recursive()` | identical |  |
| ivp_mindist_recursive | `non-virtual thunk to IVP_Mindist_Recursive::get_spawned_mindist_count()` | identical |  |
| ivp_mindist_recursive | `non-virtual thunk to IVP_Mindist_Recursive::change_spawned_mindist_count(int)` | identical |  |
| ivp_mindist_recursive | `non-virtual thunk to IVP_Mindist_Recursive::collision_is_going_to_be_deleted_event(IVP_Collision*)` | identical |  |
| ivp_mindist_recursive | `non-virtual thunk to IVP_Mindist_Recursive::~IVP_Mindist_Recursive()` | identical |  |
| ivp_object | `IVP_Real_Object::set_pinned(IVP_BOOL)` | differs | size retail 379 vs ours 377 |
| ivp_object | `IVP_Real_Object::init_object_core(IVP_Environment*, IVP_Template_Real_Object const*)` | differs | data ref at +0x136 (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_ |
| ivp_object | `IVP_Real_Object::clear_internal_references()` | differs | data ref at +0x1d (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_i |
| ivp_object | `IVP_Object::~IVP_Object()` | identical |  |
| ivp_object | `IVP_Object::~IVP_Object()` | identical |  |
| ivp_object | `IVP_Cluster::IVP_Cluster(IVP_Environment*)` | identical |  |
| ivp_object | `IVP_Cluster::~IVP_Cluster()` | identical |  |
| ivp_object | `IVP_Cluster::~IVP_Cluster()` | identical |  |
| ivp_object | `IVP_Real_Object::reset_time(IVP_Time)` | identical |  |
| ivp_object | `IVP_Real_Object::change_mass(float)` | identical |  |
| ivp_object | `IVP_Real_Object::convert_to_phantom(IVP_Template_Phantom const*)` | identical |  |
| ivp_object | `IVP_Real_Object::disable_simulation()` | identical |  |
| ivp_object | `IVP_Real_Object::add_listener_object(IVP_Listener_Object*)` | identical |  |
| ivp_object | `IVP_Real_Object::async_push_object_ws(IVP_U_Point const*, IVP_U_Float_Point const*)` | identical |  |
| ivp_object | `IVP_Real_Object::calc_m_core_f_object(IVP_U_Matrix*)` | identical |  |
| ivp_object | `IVP_Real_Object::ensure_in_simulation()` | identical |  |
| ivp_object | `IVP_Real_Object::get_all_near_mindists()` | identical |  |
| ivp_object | `IVP_Real_Object::remove_listener_object(IVP_Listener_Object*)` | identical |  |
| ivp_object | `IVP_Real_Object::set_new_m_object_f_core(IVP_U_Matrix const*)` | edited |  |
| ivp_object | `IVP_Real_Object::ensure_in_simulation_now()` | new |  |
| ivp_object | `IVP_Real_Object::recheck_collision_filter()` | identical |  |
| ivp_object | `IVP_Real_Object::async_add_speed_object_ws(IVP_U_Float_Point const*)` | identical |  |
| ivp_object | `IVP_Real_Object::delete_and_check_vicinity()` | identical |  |
| ivp_object | `IVP_Real_Object::enable_collision_detection(IVP_BOOL)` | identical |  |
| ivp_object | `IVP_Real_Object::force_grow_friction_system()` | identical |  |
| ivp_object | `IVP_Real_Object::recompile_material_changed()` | identical |  |
| ivp_object | `IVP_Real_Object::set_new_quat_object_f_core(IVP_U_Quat const*, IVP_U_Point const*)` | identical |  |
| ivp_object | `IVP_Real_Object::beam_object_to_new_position(IVP_U_Quat const*, IVP_U_Point const*, IVP_BOOL)` | identical |  |
| ivp_object | `IVP_Real_Object::revive_object_for_simulation()` | identical |  |
| ivp_object | `IVP_Real_Object::async_add_rot_speed_object_cs(IVP_U_Float_Point const*)` | identical |  |
| ivp_object | `IVP_Real_Object::revive_nearest_objects_grow_fs()` | identical |  |
| ivp_object | `IVP_Real_Object::recalc_exact_mindists_of_object()` | identical |  |
| ivp_object | `IVP_Real_Object::unlink_contact_points_for_object(IVP_Real_Object*)` | edited |  |
| ivp_object | `IVP_Real_Object::recalc_invalid_mindists_of_object()` | identical |  |
| ivp_object | `IVP_Real_Object::update_exact_mindist_events_of_object()` | identical |  |
| ivp_object | `IVP_Real_Object::IVP_Real_Object(IVP_Cluster*, IVP_SurfaceManager*, IVP_Template_Real_Object const*, IVP_U_Quat const*, IVP_U_Point const*)` | identical |  |
| ivp_object | `IVP_Real_Object::~IVP_Real_Object()` | identical |  |
| ivp_object | `IVP_Real_Object::~IVP_Real_Object()` | identical |  |
| ivp_object | `IVP_Real_Object_Fast::IVP_Real_Object_Fast(IVP_Cluster*, IVP_Template_Object const*)` | identical |  |
| ivp_object | `IVP_Real_Object_Fast::~IVP_Real_Object_Fast()` | identical |  |
| ivp_object | `IVP_Real_Object_Fast::~IVP_Real_Object_Fast()` | identical |  |
| ivp_object | `IVP_Real_Object::calc_at_matrix(IVP_Time, IVP_U_Matrix*) const` | identical |  |
| ivp_object | `IVP_Real_Object::calc_at_quaternion(IVP_Time, IVP_U_Quat*, IVP_U_Point*) const` | identical |  |
| ivp_object | `IVP_Cluster::~IVP_Cluster()` | identical |  |
| ivp_object | `IVP_Real_Object::insert_anchor(IVP_Anchor*)` | identical |  |
| ivp_object | `IVP_Real_Object::remove_anchor(IVP_Anchor*)` | identical |  |
| ivp_object | `IVP_Real_Object::delete_silently()` | identical |  |
| ivp_object | `IVP_Real_Object::get_collision_check_reference_count()` | identical |  |
| ivp_object | `IVP_Real_Object::~IVP_Real_Object()` | identical |  |
| ivp_object | `IVP_Real_Object::get_m_world_f_object_AT(IVP_U_Matrix*) const` | identical |  |
| ivp_object | `IVP_Real_Object::get_quat_world_f_object_AT(IVP_U_Quat*, IVP_U_Point*) const` | identical |  |
| ivp_object_polygon_tetra | `IVP_Object_Polygon_Tetra::make_triangles()` | differs | size retail 2305 vs ours 2337 |
| ivp_object_polygon_tetra | `IVP_Object_Polygon_Tetra::generate_double_triangle(IVP_Poly_Point*, IVP_Poly_Point*, IVP_Poly_Point*)` | differs | size retail 1184 vs ours 1188 |
| ivp_object_polygon_tetra | `P_Sur_2D::calc_triangle_representation()` | differs | data ref at +0x67 (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_c |
| ivp_object_polygon_tetra | `IVP_Triangle::calc_hesse()` | identical |  |
| ivp_object_polygon_tetra | `IVP_Triangle::~IVP_Triangle()` | identical |  |
| ivp_object_polygon_tetra | `P_Sur_2D_Line::is_crossing_line(P_Sur_2D_Line*)` | identical |  |
| ivp_object_polygon_tetra | `P_Sur_2D_Line::overlaps_with_line(P_Sur_2D_Line*)` | identical |  |
| ivp_object_polygon_tetra | `IVP_Object_Polygon_Tetra::insert_pierce_info()` | identical |  |
| ivp_object_polygon_tetra | `IVP_Object_Polygon_Tetra::IVP_Object_Polygon_Tetra(IVP_Template_Polygon*)` | identical |  |
| ivp_object_polygon_tetra | `IVP_Object_Polygon_Tetra::~IVP_Object_Polygon_Tetra()` | identical |  |
| ivp_object_polygon_tetra | `P_Sur_2D::calc_line_representation()` | identical |  |
| ivp_object_polygon_tetra | `P_Sur_2D::~P_Sur_2D()` | identical |  |
| ivp_object_polygon_tetra | `IVP_Triangle::~IVP_Triangle()` | identical |  |
| ivp_object_polygon_tetra | `IVP_Object_Polygon_Tetra::IVP_Object_Polygon_Tetra(IVP_Template_Polygon*)` | identical |  |
| ivp_object_polygon_tetra | `IVP_Object_Polygon_Tetra::~IVP_Object_Polygon_Tetra()` | identical |  |
| ivp_oo_watcher | `IVP_OO_Watcher::hull_limit_exceeded_event()` | identical |  |
| ivp_oo_watcher | `IVP_OO_Watcher::IVP_OO_Watcher(IVP_Collision_Delegator*, IVP_Real_Object*, IVP_Real_Object*)` | identical |  |
| ivp_oo_watcher | `IVP_OO_Watcher::~IVP_OO_Watcher()` | identical |  |
| ivp_oo_watcher | `IVP_OO_Watcher::~IVP_OO_Watcher()` | identical |  |
| ivp_oo_watcher | `IVP_Synapse_OO::~IVP_Synapse_OO()` | identical |  |
| ivp_oo_watcher | `IVP_Synapse_OO::~IVP_Synapse_OO()` | identical |  |
| ivp_oo_watcher | `non-virtual thunk to IVP_OO_Watcher::~IVP_OO_Watcher()` | identical |  |
| ivp_oo_watcher | `IVP_OO_Watcher::get_ledges(IVP_Compact_Ledge const**)` | identical |  |
| ivp_oo_watcher | `IVP_OO_Watcher::get_objects(IVP_Real_Object**)` | identical |  |
| ivp_oo_watcher | `IVP_OO_Watcher::collision_is_going_to_be_deleted_event(IVP_Collision*)` | identical |  |
| ivp_oo_watcher | `IVP_OO_Watcher::IVP_OO_Watcher(IVP_Collision_Delegator*, IVP_Real_Object*, IVP_Real_Object*)` | identical |  |
| ivp_oo_watcher | `IVP_OO_Watcher::~IVP_OO_Watcher()` | identical |  |
| ivp_oo_watcher | `IVP_Synapse_OO::hull_limit_exceeded_event(IVP_Hull_Manager*, float)` | identical |  |
| ivp_oo_watcher | `IVP_Synapse_OO::hull_manager_is_going_to_be_deleted_event(IVP_Hull_Manager*)` | identical |  |
| ivp_oo_watcher | `IVP_Synapse_OO::get_type()` | identical |  |
| ivp_oo_watcher | `non-virtual thunk to IVP_OO_Watcher::collision_is_going_to_be_deleted_event(IVP_Collision*)` | identical |  |
| ivp_oo_watcher | `non-virtual thunk to IVP_OO_Watcher::~IVP_OO_Watcher()` | identical |  |
| ivp_performancecounter | `IVP_PerformanceCounter::~IVP_PerformanceCounter()` | identical |  |
| ivp_performancecounter | `IVP_PerformanceCounter::~IVP_PerformanceCounter()` | identical |  |
| ivp_performancecounter | `IVP_PerformanceCounter_Simple::reset_and_print_performance_counters(IVP_Time)` | identical |  |
| ivp_performancecounter | `IVP_PerformanceCounter_Simple::IVP_PerformanceCounter_Simple()` | identical |  |
| ivp_performancecounter | `IVP_PerformanceCounter_Simple::~IVP_PerformanceCounter_Simple()` | identical |  |
| ivp_performancecounter | `IVP_PerformanceCounter_Simple::~IVP_PerformanceCounter_Simple()` | identical |  |
| ivp_performancecounter | `IVP_PerformanceCounter_Simple::stop_pcount()` | identical |  |
| ivp_performancecounter | `IVP_PerformanceCounter_Simple::start_pcount()` | identical |  |
| ivp_performancecounter | `IVP_PerformanceCounter_Simple::environment_is_going_to_be_deleted(IVP_Environment*)` | identical |  |
| ivp_performancecounter | `IVP_PerformanceCounter_Simple::pcount(IVP_PERFORMANCE_ELEMENT)` | identical |  |
| ivp_physic | `IVP_Statistic_Manager::clear_statistic()` | identical |  |
| ivp_physic | `IVP_Statistic_Manager::IVP_Statistic_Manager()` | identical |  |
| ivp_physic | `IVP_Application_Environment::IVP_Application_Environment()` | identical |  |
| ivp_physic_private | `IVP_Friction_System::dist_removed_update_pair_info(IVP_Contact_Point*)` | differs | data ref at +0x3f (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_i |
| ivp_physic_private | `IVP_Core::revive_simulation_core()` | differs | size retail 466 vs ours 338 |
| ivp_physic_private | `IVP_Environment::add_revive_core(IVP_Core*)` | identical |  |
| ivp_physic_private | `IVP_Environment::revive_cores_PSI()` | identical |  |
| ivp_physic_private | `IVP_Cluster_Manager::remove_object(IVP_Real_Object*)` | identical |  |
| ivp_physic_private | `IVP_Cluster_Manager::add_listener_object(IVP_Real_Object*, IVP_Listener_Object*)` | identical |  |
| ivp_physic_private | `IVP_Cluster_Manager::remove_listener_object(IVP_Real_Object*, IVP_Listener_Object*)` | identical |  |
| ivp_physic_private | `IVP_Cluster_Manager::fire_event_pre_collision(IVP_Real_Object*, IVP_Event_Collision*)` | identical |  |
| ivp_physic_private | `IVP_Cluster_Manager::fire_event_object_deleted(IVP_Event_Object*)` | identical |  |
| ivp_physic_private | `IVP_Cluster_Manager::fire_event_post_collision(IVP_Real_Object*, IVP_Event_Collision*)` | identical |  |
| ivp_physic_private | `IVP_Cluster_Manager::fire_event_friction_created(IVP_Real_Object*, IVP_Event_Friction*)` | identical |  |
| ivp_physic_private | `IVP_Cluster_Manager::fire_event_friction_deleted(IVP_Real_Object*, IVP_Event_Friction*)` | identical |  |
| ivp_physic_private | `IVP_Cluster_Manager::fire_event_collision_object_deleted(IVP_Real_Object*)` | identical |  |
| ivp_physic_private | `IVP_Cluster_Manager::IVP_Cluster_Manager(IVP_Environment*)` | identical |  |
| ivp_physic_private | `IVP_Friction_System::add_core_to_system(IVP_Core*)` | identical |  |
| ivp_physic_private | `IVP_Friction_System::get_controller_name()` | identical |  |
| ivp_physic_private | `IVP_Friction_System::dist_added_update_pair_info(IVP_Contact_Point*)` | identical |  |
| ivp_physic_private | `IVP_Friction_System::fs_recalc_all_contact_points()` | identical |  |
| ivp_physic_private | `IVP_Friction_System::IVP_Friction_System(IVP_Environment*)` | identical |  |
| ivp_physic_private | `IVP_Friction_System::~IVP_Friction_System()` | identical |  |
| ivp_physic_private | `IVP_Friction_System::~IVP_Friction_System()` | identical |  |
| ivp_physic_private | `IVP_Object_Callback_Table::~IVP_Object_Callback_Table()` | identical |  |
| ivp_physic_private | `IVP_Collision_Callback_Table::~IVP_Collision_Callback_Table()` | identical |  |
| ivp_physic_private | `IVP_Object_Callback_Table_Hash::find_table(IVP_Real_Object*)` | identical |  |
| ivp_physic_private | `IVP_Object_Callback_Table_Hash::remove_table(IVP_Real_Object*)` | identical |  |
| ivp_physic_private | `IVP_Collision_Callback_Table_Hash::find_table(IVP_Real_Object*)` | identical |  |
| ivp_physic_private | `IVP_Collision_Callback_Table_Hash::remove_table(IVP_Real_Object*)` | identical |  |
| ivp_physic_private | `IVP_Core::freeze_simulation_core()` | identical |  |
| ivp_physic_private | `IVP_Core::fire_event_object_frozen()` | identical |  |
| ivp_physic_private | `IVP_Environment::remove_revive_core(IVP_Core*)` | identical |  |
| ivp_physic_private | `IVP_Cluster_Manager::add_object(IVP_Real_Object*)` | identical |  |
| ivp_physic_private | `IVP_Cluster_Manager::get_root_cluster()` | identical |  |
| ivp_physic_private | `IVP_Cluster_Manager::check_for_unused_objects(IVP_Universe_Manager*)` | identical |  |
| ivp_physic_private | `IVP_Cluster_Manager::IVP_Cluster_Manager(IVP_Environment*)` | identical |  |
| ivp_physic_private | `IVP_Cluster_Manager::~IVP_Cluster_Manager()` | identical |  |
| ivp_physic_private | `IVP_Friction_System::add_dist_to_system(IVP_Contact_Point*)` | identical |  |
| ivp_physic_private | `IVP_Friction_System::get_controller_priority()` | identical |  |
| ivp_physic_private | `IVP_Friction_System::remove_core_from_system(IVP_Core*)` | identical |  |
| ivp_physic_private | `IVP_Friction_System::remove_dist_from_system(IVP_Contact_Point*)` | identical |  |
| ivp_physic_private | `IVP_Friction_System::~IVP_Friction_System()` | identical |  |
| ivp_polygon | `IVP_Polygon::IVP_Polygon(IVP_Cluster*, IVP_SurfaceManager*, IVP_Template_Real_Object const*, IVP_U_Quat const*, IVP_U_Point const*)` | identical |  |
| ivp_polygon | `IVP_Polygon::~IVP_Polygon()` | identical |  |
| ivp_polygon | `IVP_Polygon::~IVP_Polygon()` | identical |  |
| ivp_range_manager | `IVP_Range_Manager::get_coll_range_in_world(IVP_Real_Object const*)` | identical |  |
| ivp_range_manager | `IVP_Range_Manager::environment_will_be_deleted(IVP_Environment*)` | identical |  |
| ivp_range_manager | `IVP_Range_Manager::get_coll_range_intra_objects(IVP_Real_Object const*, IVP_Real_Object const*, double*, double*)` | identical |  |
| ivp_range_manager | `IVP_Range_Manager::IVP_Range_Manager(IVP_Environment*, IVP_BOOL)` | identical |  |
| ivp_ray_solver | `IVP_Ray_Solver::check_ray_against_ball(IVP_Ball*)` | identical |  |
| ivp_ray_solver | `IVP_Ray_Solver::IVP_Ray_Solver(IVP_Ray_Solver_Template const*)` | identical |  |
| ivp_ray_solver | `IVP_Ray_Solver::~IVP_Ray_Solver()` | identical |  |
| ivp_ray_solver | `IVP_Ray_Solver::~IVP_Ray_Solver()` | identical |  |
| ivp_ray_solver | `IVP_Ray_Solver_Os::check_ray_against_compact_ledge_os(IVP_Compact_Ledge const*)` | identical |  |
| ivp_ray_solver | `IVP_Ray_Solver_Os::check_ray_against_ledge_tree_node_os(IVP_Compact_Ledgetree_Node const*)` | identical |  |
| ivp_ray_solver | `IVP_Ray_Solver_Os::IVP_Ray_Solver_Os(IVP_Ray_Solver*, IVP_Real_Object*)` | edited |  |
| ivp_ray_solver | `IVP_Ray_Solver_Min::~IVP_Ray_Solver_Min()` | identical |  |
| ivp_ray_solver | `IVP_Ray_Solver_Min::~IVP_Ray_Solver_Min()` | identical |  |
| ivp_ray_solver | `IVP_Ray_Solver_Group::check_ray_group_against_node(IVP_OV_Node*, IVP_OV_Tree_Manager*)` | identical |  |
| ivp_ray_solver | `IVP_Ray_Solver_Group::IVP_Ray_Solver_Group(int, IVP_Ray_Solver**)` | identical |  |
| ivp_ray_solver | `IVP_Ray_Solver_Os::check_ray_against_compact_surface_os(IVP_Compact_Surface const*)` | identical |  |
| ivp_ray_solver | `IVP_Ray_Solver_Os::IVP_Ray_Solver_Os(IVP_Ray_Solver*, IVP_Real_Object*)` | identical |  |
| ivp_ray_solver | `IVP_Ray_Solver_Min::add_hit_object(IVP_Real_Object*, IVP_Compact_Ledge const*, IVP_Compact_Triangle const*, double, IVP_U_Point*)` | identical |  |
| ivp_ray_solver | `IVP_Ray_Solver_Group::check_ray_group_against_object(IVP_Real_Object*)` | identical |  |
| ivp_ray_solver | `IVP_Ray_Solver_Group::check_ray_group_against_all_objects_in_sim(IVP_Environment const*)` | edited |  |
| ivp_ray_solver | `IVP_Ray_Solver_Group::IVP_Ray_Solver_Group(int, IVP_Ray_Solver**)` | identical |  |
| ivp_rot_inertia_solver | `IVP_Rot_Inertia_Solver::calc_mass_center_and_rotation_inertia(IVP_Compact_Surface const*, IVP_U_Point*, IVP_U_Point*)` | differs | size retail 1653 vs ours 179 |
| ivp_rot_inertia_solver | `IVP_Compact_Ledge_Find_Mass_Center::integrate_triangle(IVP_Compact_Ledge const*, IVP_Compact_Triangle const*)` | identical |  |
| ivp_rot_inertia_solver | `IVP_Compact_Ledge_Mass_Center_Solver::integrate_triangle(IVP_Compact_Ledge const*, IVP_Compact_Triangle const*, int, int, int)` | identical |  |
| ivp_sim_unit | `_GLOBAL__I_a` | identical |  |
| ivp_sim_unit | `IVP_U_Vector<IVP_Core>::~IVP_U_Vector()` | identical |  |
| ivp_sim_unit | `IVP_Simulation_Unit::clean_sim_unit()` | identical |  |
| ivp_sim_unit | `IVP_Simulation_Unit::split_sim_unit(IVP_Core*)` | identical |  |
| ivp_sim_unit | `IVP_Simulation_Unit::add_sim_unit_core(IVP_Core*)` | identical |  |
| ivp_sim_unit | `IVP_Simulation_Unit::sim_unit_remove_core(IVP_Core*)` | identical |  |
| ivp_sim_unit | `IVP_Simulation_Unit::add_controller_of_core(IVP_Core*, IVP_Controller*)` | identical |  |
| ivp_sim_unit | `IVP_Simulation_Unit::rem_sim_unit_controller(IVP_Controller*)` | identical |  |
| ivp_sim_unit | `IVP_Simulation_Unit::sim_unit_calc_redundants()` | identical |  |
| ivp_sim_unit | `IVP_Simulation_Unit::sim_unit_union_find_test()` | identical |  |
| ivp_sim_unit | `IVP_Simulation_Unit::fusion_simulation_unities(IVP_Simulation_Unit*)` | identical |  |
| ivp_sim_unit | `IVP_Simulation_Unit::remove_controller_of_core(IVP_Core*, IVP_Controller*)` | identical |  |
| ivp_sim_unit | `IVP_Simulation_Unit::sim_unit_calc_movement_state(IVP_Environment*)` | identical |  |
| ivp_sim_unit | `IVP_Simulation_Unit::simulate_single_sim_unit_psi(IVP_Event_Sim*, IVP_U_Vector<IVP_Core>*)` | identical |  |
| ivp_sim_unit | `IVP_Simulation_Unit::sim_unit_ensure_cores_movement()` | identical |  |
| ivp_sim_unit | `IVP_Simulation_Unit::sim_unit_revive_for_simulation(IVP_Environment*)` | identical |  |
| ivp_sim_unit | `IVP_Simulation_Unit::~IVP_Simulation_Unit()` | identical |  |
| ivp_sim_unit | `IVP_Sim_Units_Manager::reset_time(IVP_Time)` | identical |  |
| ivp_sim_unit | `IVP_Controller_Manager::add_controller_to_core(IVP_Controller_Independent*, IVP_Core*)` | identical |  |
| ivp_sim_unit | `IVP_Controller_Manager::ensure_controller_in_simulation(IVP_Controller_Dependent*)` | identical |  |
| ivp_sim_unit | `IVP_Controller_Manager::announce_controller_to_environment(IVP_Controller_Dependent*)` | identical |  |
| ivp_sim_unit | `IVP_Controller_Manager::remove_controller_from_environment(IVP_Controller_Dependent*, IVP_BOOL)` | identical |  |
| ivp_sim_unit | `IVP_Standard_Gravity_Controller::get_controller_name()` | identical |  |
| ivp_sim_unit | `IVP_Standard_Gravity_Controller::do_simulation_controller(IVP_Event_Sim*, IVP_U_Vector<IVP_Core>*)` | identical |  |
| ivp_sim_unit | `IVP_Standard_Gravity_Controller::~IVP_Standard_Gravity_Controller()` | edited |  |
| ivp_sim_unit | `IVP_Standard_Gravity_Controller::~IVP_Standard_Gravity_Controller()` | identical |  |
| ivp_sim_unit | `IVP_Core::add_core_controller(IVP_Controller*)` | identical |  |
| ivp_sim_unit | `IVP_Simulation_Unit::do_sim_unit_union_find()` | identical |  |
| ivp_sim_unit | `IVP_Simulation_Unit::sim_unit_sort_controllers()` | identical |  |
| ivp_sim_unit | `IVP_Simulation_Unit::IVP_Simulation_Unit()` | identical |  |
| ivp_sim_unit | `IVP_Simulation_Unit::~IVP_Simulation_Unit()` | identical |  |
| ivp_sim_unit | `IVP_Sim_Units_Manager::simulate_sim_units_psi(IVP_Environment*, IVP_U_Vector<IVP_Core>*)` | identical |  |
| ivp_sim_unit | `IVP_Sim_Units_Manager::add_sim_unit_to_manager(IVP_Simulation_Unit*)` | identical |  |
| ivp_sim_unit | `IVP_Sim_Units_Manager::IVP_Sim_Units_Manager(IVP_Environment*)` | identical |  |
| ivp_sim_unit | `IVP_Controller_Manager::ensure_core_in_simulation(IVP_Core*)` | identical |  |
| ivp_sim_unit | `IVP_Controller_Manager::remove_controller_from_core(IVP_Controller_Independent*, IVP_Core*)` | identical |  |
| ivp_sim_unit | `IVP_Standard_Gravity_Controller::set_standard_gravity(IVP_U_Point*)` | identical |  |
| ivp_sim_unit | `IVP_Standard_Gravity_Controller::get_controller_priority()` | identical |  |
| ivp_sim_unit | `IVP_Standard_Gravity_Controller::core_is_going_to_be_deleted_event(IVP_Core*)` | identical |  |
| ivp_sim_unit | `IVP_Core::rem_core_controller(IVP_Controller*)` | identical |  |
| ivp_solver_core_reaction | `IVP_Solver_Core_Reaction::init_rot_ws(IVP_Core*, IVP_U_Point_4*, IVP_U_Point_4*, float)` | identical |  |
| ivp_solver_core_reaction | `IVP_Solver_Core_Reaction::exert_impulse_dim2(IVP_Core*, IVP_Core*, IVP_U_Float_Point&)` | identical |  |
| ivp_solver_core_reaction | `IVP_Solver_Core_Reaction::exert_angular_impulse_dim2(IVP_Core*, IVP_Core*, IVP_U_Float_Point&)` | identical |  |
| ivp_solver_core_reaction | `IVP_Solver_Core_Reaction::init_trans_ws(IVP_U_Point const*, IVP_Core*, IVP_U_Point_4*, IVP_U_Point_4*, float)` | identical |  |
| ivp_solver_core_reaction | `IVP_Solver_Core_Reaction::init_reaction_solver_rotation_ws(IVP_Core*, IVP_Core*, IVP_U_Float_Point*, IVP_U_Float_Point*, IVP_U_Float_Point*)` | identical |  |
| ivp_solver_core_reaction | `IVP_Solver_Core_Reaction::init_reaction_solver_translation_ws(IVP_Core*, IVP_Core*, IVP_U_Point&, IVP_U_Float_Point*, IVP_U_Float_Point*, IVP_U_Float_` | identical |  |
| ivp_surbuild_halfspacesoup | `IVP_SurfaceBuilder_Halfspacesoup::insert_point_into_list(IVP_U_Point*, IVP_U_Vector<IVP_U_Point>*, double)` | identical |  |
| ivp_surbuild_halfspacesoup | `IVP_SurfaceBuilder_Halfspacesoup::convert_halfspacesoup_to_points(IVP_Halfspacesoup*, double, IVP_U_Vector<IVP_U_Point>*)` | identical |  |
| ivp_surbuild_halfspacesoup | `IVP_SurfaceBuilder_Halfspacesoup::convert_halfspacesoup_to_compact_ledge(IVP_Halfspacesoup*, double)` | identical |  |
| ivp_surbuild_ledge_soup | `IVP_SurfaceBuilder_Ledge_Soup::build_root_convex_hull()` | differs | size retail 220 vs ours 192 |
| ivp_surbuild_ledge_soup | `IVP_SurfaceBuilder_Ledge_Soup::compile(IVP_Template_Surbuild_LedgeSoup*)` | differs | size retail 440 vs ours 433 |
| ivp_surbuild_ledge_soup | `IVP_SurfaceBuilder_Ledge_Soup::insert_ledge(IVP_Compact_Ledge*)` | identical |  |
| ivp_surbuild_ledge_soup | `IVP_SurfaceBuilder_Ledge_Soup::calculate_boundingbox(IVP_U_Vector<IVV_Sphere>*, IVP_U_Float_Point*, IVP_U_Float_Point*)` | identical |  |
| ivp_surbuild_ledge_soup | `IVP_SurfaceBuilder_Ledge_Soup::insert_compact_ledges()` | identical |  |
| ivp_surbuild_ledge_soup | `IVP_SurfaceBuilder_Ledge_Soup::allocate_compact_surface()` | identical |  |
| ivp_surbuild_ledge_soup | `IVP_SurfaceBuilder_Ledge_Soup::ledges_to_boxes_and_spheres()` | identical |  |
| ivp_surbuild_ledge_soup | `IVP_SurfaceBuilder_Ledge_Soup::add_ledge_tree_to_convex_hull(IVP_Compact_Recursive&, IVV_Sphere*)` | identical |  |
| ivp_surbuild_ledge_soup | `IVP_SurfaceBuilder_Ledge_Soup::cluster_spheres_topdown_mediancut(double)` | identical |  |
| ivp_surbuild_ledge_soup | `IVP_SurfaceBuilder_Ledge_Soup::recompile_point_indizes_of_compact_ledge(IVP_Compact_Ledge*, char*)` | identical |  |
| ivp_surbuild_ledge_soup | `IVP_SurfaceBuilder_Ledge_Soup::cluster_spheres_topdown_mediancut_recursively(IVP_U_Vector<IVV_Sphere>*)` | identical |  |
| ivp_surbuild_ledge_soup | `IVP_SurfaceBuilder_Ledge_Soup::cleanup()` | identical |  |
| ivp_surbuild_ledge_soup | `IVP_SurfaceBuilder_Ledge_Soup::IVP_SurfaceBuilder_Ledge_Soup()` | identical |  |
| ivp_surbuild_ledge_soup | `IVP_SurfaceBuilder_Ledge_Soup::~IVP_SurfaceBuilder_Ledge_Soup()` | identical |  |
| ivp_surbuild_ledge_soup | `IVP_SurfaceBuilder_Ledge_Soup::build_ledgetree(IVV_Sphere*)` | identical |  |
| ivp_surbuild_ledge_soup | `IVP_SurfaceBuilder_Ledge_Soup::~IVP_SurfaceBuilder_Ledge_Soup()` | identical |  |
| ivp_surbuild_pointsoup | `IVP_SurfaceBuilder_Pointsoup::convert_pointsoup_to_compact_ledge_internal(IVP_U_Vector<IVP_U_Point>*)` | differs | bytes |
| ivp_surbuild_pointsoup | `IVP_SurfaceBuilder_Pointsoup::try_to_build_convex_ledge_from_qhull_result(IVP_U_Vector<IVP_U_Point>*, IVP_BOOL*, char*, char*)` | differs | size retail 1482 vs ours 1418 |
| ivp_surbuild_pointsoup | `IVP_SurMan_PS_Plane::get_area_size()` | identical |  |
| ivp_surbuild_pointsoup | `IVP_SurfaceBuilder_Pointsoup::planes_to_template(IVP_U_Vector<IVP_U_Point>*, IVP_U_Vector<IVP_SurMan_PS_Plane>*)` | identical |  |
| ivp_surbuild_pointsoup | `IVP_SurfaceBuilder_Pointsoup::convert_triangle_to_compace_ledge(IVP_U_Point*, IVP_U_Point*, IVP_U_Point*)` | identical |  |
| ivp_surbuild_pointsoup | `IVP_SurfaceBuilder_Pointsoup::convert_pointsoup_to_compact_ledge(IVP_U_Vector<IVP_U_Point>*)` | identical |  |
| ivp_surbuild_polygon_convex | `IVP_SurfaceBuilder_Polygon_Convex::convert_template_to_ledge(IVP_Template_Polygon*)` | identical |  |
| ivp_surbuild_polygon_convex | `IVP_SurfaceBuilder_Polygon_Convex::init_surface_manager_polygon()` | identical |  |
| ivp_surbuild_polygon_convex | `IVP_SurfaceBuilder_Polygon_Convex::convert_templateledgepolygonsoup_to_ledge(IVP_Template_Ledge_Polygon_Soup*)` | identical |  |
| ivp_surbuild_polygon_convex | `IVP_SurfaceBuilder_Polygon_Convex::IVP_SurfaceBuilder_Polygon_Convex(IVP_Template_Polygon*)` | identical |  |
| ivp_surbuild_polygon_convex | `IVP_SurfaceBuilder_Polygon_Convex::IVP_SurfaceBuilder_Polygon_Convex(IVP_Template_Ledge_Polygon_Soup*)` | identical |  |
| ivp_surface_manager | `IVP_SurfaceManager::~IVP_SurfaceManager()` | identical |  |
| ivp_surface_manager | `IVP_SurfaceManager::add_reference_to_ledge(IVP_Compact_Ledge const*)` | identical |  |
| ivp_surface_manager | `IVP_SurfaceManager::remove_reference_to_ledge(IVP_Compact_Ledge const*)` | identical |  |
| ivp_surman_polygon | `IVP_SurfaceManager_Polygon::get_all_terminal_ledges(IVP_U_BigVector<IVP_Compact_Ledge>*)` | identical |  |
| ivp_surman_polygon | `IVP_SurfaceManager_Polygon::insert_all_ledges_hitting_ray(IVP_Ray_Solver*, IVP_Real_Object*)` | identical |  |
| ivp_surman_polygon | `IVP_SurfaceManager_Polygon::~IVP_SurfaceManager_Polygon()` | identical |  |
| ivp_surman_polygon | `IVP_SurfaceManager_Polygon::~IVP_SurfaceManager_Polygon()` | identical |  |
| ivp_surman_polygon | `IVP_SurfaceManager_Polygon_Solver::traverse_cluster(IVP_Compact_Ledgetree_Node const*, IVP_U_Point const*, double, IVP_U_BigVector<IVP_Compact_Ledge>*` | identical |  |
| ivp_surman_polygon | `IVP_SurfaceManager_Polygon::get_radius_and_radius_dev_to_given_center(IVP_U_Float_Point const*, float*, float*) const` | identical |  |
| ivp_surman_polygon | `IVP_SurfaceManager_Polygon::add_reference_to_ledge(IVP_Compact_Ledge const*)` | identical |  |
| ivp_surman_polygon | `IVP_SurfaceManager_Polygon::remove_reference_to_ledge(IVP_Compact_Ledge const*)` | identical |  |
| ivp_surman_polygon | `IVP_SurfaceManager_Polygon::get_all_ledges_within_radius(IVP_U_Point const*, double, IVP_Compact_Ledge const*, IVP_Real_Object*, IVP_Compact_Ledge con` | identical |  |
| ivp_surman_polygon | `IVP_SurfaceManager_Polygon::get_type()` | identical |  |
| ivp_surman_polygon | `IVP_SurfaceManager_Polygon::get_mass_center(IVP_U_Float_Point*) const` | identical |  |
| ivp_surman_polygon | `IVP_SurfaceManager_Polygon::get_single_convex() const` | identical |  |
| ivp_surman_polygon | `IVP_SurfaceManager_Polygon::get_rotation_inertia(IVP_U_Float_Point*) const` | identical |  |
| ivp_template_constraint | `IVP_Environment::create_constraint(IVP_Template_Constraint const*)` | identical |  |
| ivp_template_constraint | `IVP_Template_Constraint::IVP_Template_Constraint()` | edited |  |
| ivp_template_constraint | `IVP_Template_Constraint::fix_rotation_axis(IVP_COORDINATE_INDEX)` | identical |  |
| ivp_template_constraint | `IVP_Template_Constraint::free_rotation_axis(IVP_COORDINATE_INDEX)` | identical |  |
| ivp_template_constraint | `IVP_Template_Constraint::set_attached_object(IVP_Real_Object*)` | identical |  |
| ivp_template_constraint | `IVP_Template_Constraint::set_reference_object(IVP_Real_Object*)` | identical |  |
| ivp_template_constraint | `IVP_Template_Constraint::free_translation_axis(IVP_COORDINATE_INDEX)` | identical |  |
| ivp_templates | `IVP_Template_Object::set_name(char const*)` | identical |  |
| ivp_templates | `IVP_Template_Real_Object::IVP_Template_Real_Object()` | identical |  |
| ivp_templates | `IVP_Template_Real_Object::~IVP_Template_Real_Object()` | identical |  |
| ivp_templates_intern | `IVP_Template_Polygon::IVP_Template_Polygon(int, int, int)` | identical |  |
| ivp_templates_intern | `IVP_Template_Polygon::~IVP_Template_Polygon()` | identical |  |
| ivp_templates_intern | `IVP_Template_Surface::init_surface(int)` | identical |  |
| ivp_templates_intern | `IVP_Template_Surface::close_surface()` | identical |  |
| ivp_templates_intern | `IVP_Template_Surface::~IVP_Template_Surface()` | identical |  |
| ivp_templates_intern | `IVP_Template_Polygon::IVP_Template_Polygon(int, int, int)` | identical |  |
| ivp_templates_intern | `IVP_Template_Polygon::IVP_Template_Polygon()` | identical |  |
| ivp_templates_intern | `IVP_Template_Polygon::~IVP_Template_Polygon()` | identical |  |
| ivp_templates_intern | `IVP_Template_Surface::get_surface_index()` | identical |  |
| ivp_templates_intern | `IVP_Template_Surface::IVP_Template_Surface()` | identical |  |
| ivp_time | `IVP_Time_Manager::remove_event(IVP_Time_Event*)` | identical |  |
| ivp_time | `IVP_Time_Manager::update_event(IVP_Time_Event*, IVP_Time)` | new |  |
| ivp_time | `IVP_Time_Manager::env_set_current_time(IVP_Environment*, IVP_Time)` | identical |  |
| ivp_time | `IVP_Time_Manager::IVP_Time_Manager()` | edited |  |
| ivp_time | `IVP_Time_Manager::~IVP_Time_Manager()` | identical |  |
| ivp_time | `IVP_Event_Manager::simulate_variable_time_step(IVP_Time_Manager*, IVP_Environment*, IVP_Time_Event_PSI*, float)` | identical |  |
| ivp_time | `IVP_Time_Event_PSI::simulate_time_event(IVP_Environment*)` | edited |  |
| ivp_time | `IVP_Event_Manager_Standard::simulate_time_events(IVP_Time_Manager*, IVP_Environment*, IVP_Time)` | identical |  |
| ivp_time | `IVP_Event_Manager_Standard::~IVP_Event_Manager_Standard()` | identical |  |
| ivp_time | `IVP_Time_Manager::event_loop(IVP_Environment*, IVP_Time)` | identical |  |
| ivp_time | `IVP_Time_Manager::reset_time(IVP_Time)` | edited |  |
| ivp_time | `IVP_Time_Manager::IVP_Time_Manager()` | identical |  |
| ivp_time | `IVP_Time_Manager::~IVP_Time_Manager()` | identical |  |
| ivp_time | `IVP_Event_Manager_Standard::~IVP_Event_Manager_Standard()` | identical |  |
| ivu_active_value | `IVP_U_Active_Int::~IVP_U_Active_Int()` | identical |  |
| ivu_active_value | `IVP_U_Active_Int::~IVP_U_Active_Int()` | identical |  |
| ivu_active_value | `IVP_U_Active_Float::add_dependency(IVP_U_Active_Float_Listener*)` | identical |  |
| ivu_active_value | `IVP_U_Active_Float::~IVP_U_Active_Float()` | identical |  |
| ivu_active_value | `IVP_U_Active_Float::~IVP_U_Active_Float()` | identical |  |
| ivu_active_value | `IVP_U_Active_Value::~IVP_U_Active_Value()` | identical |  |
| ivu_active_value | `IVP_U_Active_Value::~IVP_U_Active_Value()` | identical |  |
| ivu_active_value | `IVP_Active_Value_Hash::~IVP_Active_Value_Hash()` | identical |  |
| ivu_active_value | `IVP_Active_Value_Hash::~IVP_Active_Value_Hash()` | identical |  |
| ivu_active_value | `IVP_U_Active_Terminal_Int::print()` | identical |  |
| ivu_active_value | `IVP_U_Active_Terminal_Int::set_int(int, IVP_BOOL)` | identical |  |
| ivu_active_value | `IVP_U_Active_Terminal_Int::~IVP_U_Active_Terminal_Int()` | identical |  |
| ivu_active_value | `IVP_U_Active_Value_Manager::delay_active_int(IVP_U_Active_Int_Delayed*)` | identical |  |
| ivu_active_value | `IVP_U_Active_Value_Manager::create_active_int(char const*, int)` | edited |  |
| ivu_active_value | `IVP_U_Active_Value_Manager::insert_active_int(IVP_U_Active_Int*)` | edited |  |
| ivu_active_value | `IVP_U_Active_Value_Manager::remove_active_int(IVP_U_Active_Int*)` | edited |  |
| ivu_active_value | `IVP_U_Active_Value_Manager::delay_active_float(IVP_U_Active_Float_Delayed*)` | identical |  |
| ivu_active_value | `IVP_U_Active_Value_Manager::install_active_int(char const*, int)` | edited |  |
| ivu_active_value | `IVP_U_Active_Value_Manager::create_active_float(char const*, double)` | edited |  |
| ivu_active_value | `IVP_U_Active_Value_Manager::insert_active_float(IVP_U_Active_Float*)` | edited |  |
| ivu_active_value | `IVP_U_Active_Value_Manager::remove_active_float(IVP_U_Active_Float*)` | edited |  |
| ivu_active_value | `IVP_U_Active_Value_Manager::install_active_float(char const*, double)` | edited |  |
| ivu_active_value | `IVP_U_Active_Value_Manager::init_active_values_generic()` | identical |  |
| ivu_active_value | `IVP_U_Active_Value_Manager::update_delayed_active_values()` | identical |  |
| ivu_active_value | `IVP_U_Active_Value_Manager::IVP_U_Active_Value_Manager(IVP_BOOL)` | identical |  |
| ivu_active_value | `IVP_U_Active_Value_Manager::~IVP_U_Active_Value_Manager()` | identical |  |
| ivu_active_value | `IVP_U_Active_Value_Manager::~IVP_U_Active_Value_Manager()` | identical |  |
| ivu_active_value | `IVP_U_Active_Terminal_Double::set_double(double, IVP_BOOL)` | identical |  |
| ivu_active_value | `IVP_U_Active_Terminal_Double::print()` | identical |  |
| ivu_active_value | `IVP_U_Active_Terminal_Double::~IVP_U_Active_Terminal_Double()` | identical |  |
| ivu_active_value | `IVP_Active_Value_Hash::compare(void*, void*) const` | identical |  |
| ivu_active_value | `IVP_U_Active_Int::~IVP_U_Active_Int()` | identical |  |
| ivu_active_value | `IVP_U_Active_Float::remove_dependency(IVP_U_Active_Float_Listener*)` | identical |  |
| ivu_active_value | `IVP_U_Active_Float::~IVP_U_Active_Float()` | identical |  |
| ivu_active_value | `IVP_U_Active_Terminal_Int::update_int()` | identical |  |
| ivu_active_value | `IVP_U_Active_Terminal_Int::~IVP_U_Active_Terminal_Int()` | identical |  |
| ivu_active_value | `IVP_U_Active_Value_Manager::refresh_psi_active_values(IVP_Environment*)` | identical |  |
| ivu_active_value | `IVP_U_Active_Value_Manager::environment_will_be_deleted(IVP_Environment*)` | identical |  |
| ivu_active_value | `IVP_U_Active_Value_Manager::IVP_U_Active_Value_Manager(IVP_BOOL)` | identical |  |
| ivu_active_value | `IVP_U_Active_Value_Manager::~IVP_U_Active_Value_Manager()` | identical |  |
| ivu_active_value | `IVP_U_Active_Terminal_Double::update_float()` | identical |  |
| ivu_active_value | `IVP_U_Active_Terminal_Double::~IVP_U_Active_Terminal_Double()` | identical |  |
| ivu_active_value | `non-virtual thunk to IVP_U_Active_Terminal_Int::update_int()` | identical |  |
| ivu_active_value | `non-virtual thunk to IVP_U_Active_Terminal_Double::update_float()` | identical |  |
| ivu_bigvector | `IVP_U_BigVector_Base::increment_mem()` | identical |  |
| ivu_geometry | `IVP_U_Plain::IVP_U_Plain(IVP_U_Hesse const*)` | edited |  |
| ivu_geometry | `IVP_U_Plain::calc_intersect_with(IVP_U_Hesse const*, IVP_U_Straight*) const` | identical |  |
| ivu_hash | `IVP_Hash::add(char const*, void*)` | identical |  |
| ivu_hash | `IVP_Hash::IVP_Hash(int, int, void*)` | identical |  |
| ivu_hash | `IVP_Hash::~IVP_Hash()` | identical |  |
| ivu_hash | `IVP_Hash::find(char const*) const` | identical |  |
| ivu_linear | `IVP_U_Hesse::calc_hesse(IVP_U_Point const*, IVP_U_Point const*, IVP_U_Point const*)` | identical |  |
| ivu_linear | `IVP_U_Hesse::calc_hesse(IVP_U_Float_Point const*, IVP_U_Float_Point const*, IVP_U_Float_Point const*)` | identical |  |
| ivu_linear | `IVP_U_Hesse::normize()` | identical |  |
| ivu_linear | `IVP_U_Point::fast_normize()` | edited |  |
| ivu_linear | `IVP_U_Point::set_crossing(IVP_U_Hesse*, IVP_U_Hesse*, IVP_U_Hesse*)` | identical |  |
| ivu_linear | `IVP_U_Point::set_interpolate(IVP_U_Float_Point const*, IVP_U_Float_Point const*, double)` | identical |  |
| ivu_linear | `IVP_U_Point::set_interpolate(IVP_U_Point const*, IVP_U_Point const*, double)` | identical |  |
| ivu_linear | `IVP_U_Point::calc_an_orthogonal(IVP_U_Point const*)` | identical |  |
| ivu_linear | `IVP_U_Point::real_length_plus_normize()` | edited |  |
| ivu_linear | `IVP_U_Point::solve_quadratic_equation_accurate(IVP_U_Point const*)` | identical |  |
| ivu_linear | `IVP_U_Point::rotate(IVP_COORDINATE_INDEX, float)` | identical |  |
| ivu_linear | `IVP_U_Point::normize()` | edited |  |
| ivu_linear | `IVP_U_Matrix::real_invert(double)` | identical |  |
| ivu_linear | `IVP_U_Matrix::set_transpose(IVP_U_Matrix const*)` | identical |  |
| ivu_linear | `IVP_U_Matrix::init_rot_multiple(IVP_U_Point const*, double)` | identical |  |
| ivu_linear | `IVP_U_Matrix3::init_normized3_col(IVP_U_Point const*, IVP_COORDINATE_INDEX)` | edited |  |
| ivu_linear | `IVP_U_Matrix3::init_normized3_col(IVP_U_Point const*, IVP_COORDINATE_INDEX, IVP_U_Point const*)` | edited |  |
| ivu_linear | `IVP_U_Float_Hesse::calc_hesse_val(IVP_U_Float_Point const*)` | identical |  |
| ivu_linear | `IVP_U_Float_Hesse::normize()` | identical |  |
| ivu_linear | `IVP_U_Float_Point::fast_normize()` | edited |  |
| ivu_linear | `IVP_U_Float_Point::real_length_plus_normize()` | edited |  |
| ivu_linear | `IVP_U_Float_Point::rotate(IVP_COORDINATE_INDEX, float)` | identical |  |
| ivu_linear | `IVP_U_Float_Point::normize()` | edited |  |
| ivu_linear | `IVP_U_Hesse::proj_on_plane(IVP_U_Point const*, IVP_U_Point*) const` | identical |  |
| ivu_linear | `IVP_U_Point::is_parallel(IVP_U_Point const*, double) const` | identical |  |
| ivu_linear | `IVP_U_Float_Hesse::proj_on_plane(IVP_U_Float_Point const*, IVP_U_Float_Point*) const` | identical |  |
| ivu_linear | `IVP_U_Point::calc_cross_product(IVP_U_Point const*, IVP_U_Point const*)` | identical |  |
| ivu_linear | `IVP_U_Point::line_max(IVP_U_Point const*)` | identical |  |
| ivu_linear | `IVP_U_Point::line_min(IVP_U_Point const*)` | identical |  |
| ivu_linear | `IVP_U_Matrix::init()` | identical |  |
| ivu_linear | `IVP_U_Matrix::shift_os(IVP_U_Point const*)` | identical |  |
| ivu_linear | `IVP_U_Matrix3::transpose3()` | identical |  |
| ivu_linear | `IVP_U_Matrix3::set_transpose3(IVP_U_Matrix3 const*)` | identical |  |
| ivu_linear | `IVP_U_Matrix3::init3()` | identical |  |
| ivu_linear | `IVP_U_Float_Point::calc_cross_product(IVP_U_Float_Point const*, IVP_U_Float_Point const*)` | identical |  |
| ivu_linear | `IVP_U_Float_Point::set_orthogonal_part(IVP_U_Float_Point const*, IVP_U_Float_Point const*)` | identical |  |
| ivu_linear | `IVP_U_Point::real_length() const` | identical |  |
| ivu_linear | `IVP_U_Matrix::mmult4(IVP_U_Matrix const*, IVP_U_Matrix*) const` | identical |  |
| ivu_linear | `IVP_U_Matrix::vmult4(IVP_U_Point const*, IVP_U_Point*) const` | identical |  |
| ivu_linear | `IVP_U_Matrix::vmult4(IVP_U_Float_Point const*, IVP_U_Point*) const` | identical |  |
| ivu_linear | `IVP_U_Matrix::vmult4(IVP_U_Float_Point const*, IVP_U_Float_Point*) const` | identical |  |
| ivu_linear | `IVP_U_Matrix::mimult4(IVP_U_Matrix const*, IVP_U_Matrix*) const` | identical |  |
| ivu_linear | `IVP_U_Matrix::vimult4(IVP_U_Point const*, IVP_U_Float_Point*) const` | identical |  |
| ivu_linear | `IVP_U_Matrix::vimult4(IVP_U_Point const*, IVP_U_Point*) const` | identical |  |
| ivu_linear | `IVP_U_Matrix::vimult4(IVP_U_Float_Point const*, IVP_U_Float_Point*) const` | identical |  |
| ivu_linear | `IVP_U_Matrix::mi2mult4(IVP_U_Matrix const*, IVP_U_Matrix*) const` | identical |  |
| ivu_linear | `IVP_U_Matrix3::mmult3(IVP_U_Matrix3 const*, IVP_U_Matrix3*) const` | identical |  |
| ivu_linear | `IVP_U_Matrix3::vmult3(IVP_U_Point const*, IVP_U_Point*) const` | identical |  |
| ivu_linear | `IVP_U_Matrix3::vmult3(IVP_U_Float_Point const*, IVP_U_Float_Point*) const` | identical |  |
| ivu_linear | `IVP_U_Matrix3::mimult3(IVP_U_Matrix3 const*, IVP_U_Matrix3*) const` | identical |  |
| ivu_linear | `IVP_U_Matrix3::vimult3(IVP_U_Point const*, IVP_U_Point*) const` | identical |  |
| ivu_linear | `IVP_U_Matrix3::vimult3(IVP_U_Float_Point const*, IVP_U_Float_Point*) const` | identical |  |
| ivu_linear | `IVP_U_Matrix3::mi2mult3(IVP_U_Matrix3 const*, IVP_U_Matrix3*) const` | identical |  |
| ivu_linear | `IVP_U_Float_Point::real_length() const` | identical |  |
| ivu_linear | `IVP_U_Float_Point::fast_real_length() const` | identical |  |
| ivu_memory | `ivp_free_aligned(void*)` | identical |  |
| ivu_memory | `ivp_malloc_aligned(int, int)` | identical |  |
| ivu_memory | `p_free(void*)` | identical |  |
| ivu_memory | `p_calloc(int, int)` | identical |  |
| ivu_memory | `p_malloc(unsigned int)` | identical |  |
| ivu_memory | `IVP_U_Memory::neuer_sp_block(unsigned int)` | identical |  |
| ivu_memory | `IVP_U_Memory::free_mem_transaction()` | identical |  |
| ivu_memory | `IVP_U_Memory::init_mem_transaction_usage(char*, int)` | identical |  |
| ivu_memory | `IVP_U_Memory::~IVP_U_Memory()` | identical |  |
| ivu_memory | `IVP_U_Memory::IVP_U_Memory()` | identical |  |
| ivu_min_hash | `IVP_U_Min_Hash::add(void*, double)` | identical |  |
| ivu_min_hash | `IVP_U_Min_Hash::remove(void*)` | identical |  |
| ivu_min_hash | `IVP_U_Min_Hash::~IVP_U_Min_Hash()` | identical |  |
| ivu_min_hash | `IVP_U_Min_Hash::remove_min()` | identical |  |
| ivu_min_hash | `IVP_U_Min_Hash::min_removed_at_index(IVP_U_Min_Hash_Elem*, int)` | identical |  |
| ivu_min_hash | `IVP_U_Min_Hash::IVP_U_Min_Hash(int)` | identical |  |
| ivu_min_hash | `IVP_U_Min_Hash::~IVP_U_Min_Hash()` | identical |  |
| ivu_min_list | `IVP_U_Min_List::~IVP_U_Min_List()` | identical |  |
| ivu_min_list | `IVP_U_Min_List::remove_minlist_elem(unsigned int)` | identical |  |
| ivu_min_list | `IVP_U_Min_List::add(void*, float)` | identical |  |
| ivu_min_list | `IVP_U_Min_List::IVP_U_Min_List(int)` | identical |  |
| ivu_os_dep | `ivp_rand()` | identical |  |
| ivu_os_dep | `ivp_srand(int)` | identical |  |
| ivu_quat | `IVP_U_Quat::normize_quat()` | identical |  |
| ivu_quat | `IVP_U_Quat::set_quaternion(IVP_U_Matrix3 const*)` | identical |  |
| ivu_quat | `IVP_U_Quat::set_invert_mult(IVP_U_Quat const*, IVP_U_Quat const*)` | identical |  |
| ivu_quat | `IVP_U_Quat::fast_normize_quat()` | identical |  |
| ivu_quat | `IVP_U_Quat::set_div_unit_quat(IVP_U_Quat const*, IVP_U_Quat const*)` | identical |  |
| ivu_quat | `IVP_U_Quat::set_invert_unit_quat(IVP_U_Quat const*)` | identical |  |
| ivu_quat | `IVP_U_Quat::set_very_fast_multiple(IVP_U_Float_Point const*, double)` | identical |  |
| ivu_quat | `IVP_U_Quat::set_interpolate_smoothly(IVP_U_Quat const*, IVP_U_Quat const*, double)` | identical |  |
| ivu_quat | `IVP_U_Quat::set_fast_multiple_with_clip(IVP_U_Float_Point const*, double)` | identical |  |
| ivu_quat | `IVP_U_Quat::set_matrix(IVP_U_Matrix3*) const` | identical |  |
| ivu_quat | `IVP_U_Quat::set_mult_quat(IVP_U_Quat const*, IVP_U_Quat const*)` | identical |  |
| ivu_string | `ivp_message(char const*, ...)` | edited |  |
| ivu_string | `p_strcmp(char const*, char const*)` | identical |  |
| ivu_string | `p_strdup(char const*)` | edited |  |
| ivu_string | `p_strlen(char const*)` | identical |  |
| ivu_vector | `IVP_U_Vector_Base::increment_mem()` | identical |  |
| ivu_vhash | `IVP_VHash::remove_elem(void const*, unsigned int)` | differs | data ref at +0x17 (string): retail "/Users/buildbot/buildslave/rel_singleplayer_osx/build/src/iv...", ours "../ivp/ivp_u |
| ivu_vhash | `IVP_VHash_Store::change_elem(void*, void*)` | identical |  |
| ivu_vhash | `IVP_VHash_Store::remove_elem(void*)` | identical |  |
| ivu_vhash | `IVP_VHash_Store::add_elem(void*, void*)` | identical |  |
| ivu_vhash | `IVP_VHash_Store::find_elem(void*)` | identical |  |
| ivu_vhash | `IVP_VHash_Store::~IVP_VHash_Store()` | identical |  |
| ivu_vhash | `IVP_VHash_Store::~IVP_VHash_Store()` | identical |  |
| ivu_vhash | `IVP_VHash::IVP_VHash(int)` | identical |  |
| ivu_vhash | `IVP_VHash::~IVP_VHash()` | identical |  |
| ivu_vhash | `IVP_VHash::~IVP_VHash()` | identical |  |
| ivu_vhash | `IVP_VHash::~IVP_VHash()` | identical |  |
| ivu_vhash | `IVP_VHash_Store::remove_elem(void*, unsigned int)` | identical |  |
| ivu_vhash | `IVP_VHash_Store::rehash(int)` | identical |  |
| ivu_vhash | `IVP_VHash_Store::IVP_VHash_Store(int)` | identical |  |
| ivu_vhash | `IVP_VHash_Store::IVP_VHash_Store(int)` | identical |  |
| ivu_vhash | `IVP_VHash::rehash(int)` | identical |  |
| ivu_vhash | `IVP_VHash::add_elem(void const*, int)` | identical |  |
| ivu_vhash | `IVP_VHash::find_elem(void const*, unsigned int) const` | identical |  |
| ledgewriter | `PackLedgeIntoBuffer(packedhull_t*, CUtlBuffer&, IVP_Compact_Ledge const*, virtualmeshlist_t const&)` | differs | bytes |
| ledgewriter | `BuildVertMap(vertmap_t&, Vector const*, int, IVP_Compact_Ledge const*)` | identical |  |
| ledgewriter | `CUtlMemory<int, int>::Grow(int)` | identical |  |
| ledgewriter | `CVPhysicsVirtualMeshWriter::LedgeCanBePacked(IVP_Compact_Ledge const*, virtualmeshlist_t const&)` | identical |  |
| ledgewriter | `CVPhysicsVirtualMeshWriter::DestroyPackedHull(virtualmeshhull_t*)` | identical |  |
| ledgewriter | `CVPhysicsVirtualMeshWriter::InitTwoSidedTriangleLege(triangleledge_t*, IVP_Compact_Poly_Point const*, int, int, int, int)` | identical |  |
| ledgewriter | `CVPhysicsVirtualMeshWriter::CreatePackedHullFromLedges(virtualmeshlist_t const&, IVP_Compact_Ledge const**, int)` | identical |  |
| ledgewriter | `CVPhysicsVirtualMeshWriter::UnpackCompactLedgeFromHull(IVP_Compact_Ledge*, int, IVP_Compact_Poly_Point const*, virtualmeshhull_t const*, int, bool)` | identical |  |
| ledgewriter | `CVPhysicsVirtualMeshWriter::UnpackLedgeListFromHull(unsigned char*, virtualmeshhull_t*, IVP_Compact_Poly_Point*)` | identical |  |
| local_constraint_system | `hk_Local_Constraint_System::apply_effector_PSI(hk_PSI_Info&, hk_Array<hk_Rigid_Body*>*)` | differs | size retail 1405 vs ours 1421 |
| local_constraint_system | `hk_Link_EF::get_controller_name()` | identical |  |
| local_constraint_system | `hk_Link_EF::~hk_Link_EF()` | identical |  |
| local_constraint_system | `hk_Link_EF::~hk_Link_EF()` | identical |  |
| local_constraint_system | `hk_Local_Constraint_System::deactivate()` | identical |  |
| local_constraint_system | `hk_Local_Constraint_System::add_constraint(hk_Constraint*, int)` | identical |  |
| local_constraint_system | `hk_Local_Constraint_System::sort_constraints()` | identical |  |
| local_constraint_system | `hk_Local_Constraint_System::get_constraints_in_system(hk_Array<hk_Constraint*>&)` | identical |  |
| local_constraint_system | `hk_Local_Constraint_System::activate()` | identical |  |
| local_constraint_system | `hk_Local_Constraint_System::hk_Local_Constraint_System(hk_Environment*, hk_Local_Constraint_System_BP*)` | identical |  |
| local_constraint_system | `hk_Local_Constraint_System::~hk_Local_Constraint_System()` | identical |  |
| local_constraint_system | `hk_Local_Constraint_System::~hk_Local_Constraint_System()` | identical |  |
| local_constraint_system | `hk_Link_EF::get_controller_priority()` | identical |  |
| local_constraint_system | `hk_Link_EF::do_simulation_controller(IVP_Event_Sim*, IVP_U_Vector<IVP_Core>*)` | identical |  |
| local_constraint_system | `hk_Link_EF::anchor_will_be_deleted_event(IVP_Anchor*)` | identical |  |
| local_constraint_system | `hk_Link_EF::get_associated_controlled_cores()` | identical |  |
| local_constraint_system | `hk_Link_EF::core_is_going_to_be_deleted_event(IVP_Core*)` | identical |  |
| local_constraint_system | `hk_Local_Constraint_System::solve_penetration(IVP_Real_Object*, IVP_Real_Object*)` | identical |  |
| local_constraint_system | `hk_Local_Constraint_System::write_to_blueprint(hk_Local_Constraint_System_BP*)` | identical |  |
| local_constraint_system | `hk_Local_Constraint_System::report_square_error(float)` | identical |  |
| local_constraint_system | `hk_Local_Constraint_System::entity_deletion_event(hk_Rigid_Body*)` | identical |  |
| local_constraint_system | `hk_Local_Constraint_System::get_effector_priority()` | identical |  |
| local_constraint_system | `hk_Local_Constraint_System::constraint_deletion_event(hk_Constraint*)` | identical |  |
| local_constraint_system | `hk_Local_Constraint_System::core_is_going_to_be_deleted_event(IVP_Core*)` | identical |  |
| local_constraint_system | `hk_Local_Constraint_System::~hk_Local_Constraint_System()` | identical |  |
| main | `_GLOBAL__I_a` | identical |  |
| main | `vphysics_init()` | identical |  |
| main | `__CreateCPhysicsInterfaceIPhysics_interface()` | identical |  |
| main | `CUtlMemory<CPhysicsCollisionSet, int>::Grow(int)` | identical |  |
| main | `CUtlMemory<IPhysicsEnvironment*, int>::Grow(int)` | identical |  |
| main | `CTier1AppSystem<IPhysics, 0>::Disconnect()` | identical |  |
| main | `CTier1AppSystem<IPhysics, 0>::Init()` | identical |  |
| main | `CTier1AppSystem<IPhysics, 0>::Connect(void* (*)(char const*, int*))` | identical |  |
| main | `CTier1AppSystem<IPhysics, 0>::Shutdown()` | identical |  |
| main | `CPhysicsInterface::QueryInterface(char const*)` | identical |  |
| main | `CPhysicsInterface::FindCollisionSet(unsigned int)` | identical |  |
| main | `CPhysicsInterface::CreateEnvironment()` | identical |  |
| main | `CPhysicsInterface::DestroyEnvironment(IPhysicsEnvironment*)` | identical |  |
| main | `CPhysicsInterface::CreateObjectPairHash()` | identical |  |
| main | `CPhysicsInterface::DestroyAllCollisionSets()` | identical |  |
| main | `CPhysicsInterface::FindOrCreateCollisionSet(unsigned int, int)` | identical |  |
| main | `CPhysicsInterface::~CPhysicsInterface()` | identical |  |
| main | `CPhysicsInterface::DestroyObjectPairHash(IPhysicsObjectPairHash*)` | identical |  |
| main | `CPhysicsInterface::GetActiveEnvironmentByIndex(int)` | identical |  |
| main | `CPhysicsCollisionSet::ShouldCollide(int, int)` | identical |  |
| main | `CPhysicsCollisionSet::EnableCollisions(int, int)` | identical |  |
| main | `CPhysicsCollisionSet::DisableCollisions(int, int)` | identical |  |
| math | `hk_Math::srand01(unsigned int)` | identical |  |
| mathlib_base | `AngleMatrix(QAngle const&, Vector const&, matrix3x4_t&)` | identical |  |
| mathlib_base | `AngleVectors(QAngle const&, Vector*, Vector*, Vector*)` | identical |  |
| mathlib_base | `MathLib_Init(float, float, float, int, bool, bool, bool, bool)` | identical |  |
| mathlib_base | `MatrixAngles(matrix3x4_t const&, float*)` | identical |  |
| mathlib_base | `_InvRSquared(float const*)` | identical |  |
| mathlib_base | `ConcatTransforms(matrix3x4_t const&, matrix3x4_t const&, matrix3x4_t&)` | identical |  |
| mathlib_base | `_VectorNormalize(Vector&)` | identical |  |
| mathlib_base | `QuaternionAxisAngle(Quaternion const&, Vector&, float&)` | identical |  |
| mathlib_base | `_VectorNormalizeFast(Vector&)` | identical |  |
| mathlib_base | `SinCos(float, float*, float*)` | identical |  |
| mathlib_base | `_rsqrtf(float)` | identical |  |
| mathlib_base | `ClearBounds(Vector&, Vector&)` | identical |  |
| mathlib_base | `VectorRotate(float const*, matrix3x4_t const&, float*)` | identical |  |
| mathlib_base | `VectorIRotate(float const*, matrix3x4_t const&, float*)` | identical |  |
| mathlib_base | `MatrixGetColumn(matrix3x4_t const&, int, Vector&)` | identical |  |
| mathlib_base | `VectorTransform(float const*, matrix3x4_t const&, float*)` | identical |  |
| mathlib_base | `AddPointToBounds(Vector const&, Vector&, Vector&)` | identical |  |
| mathlib_base | `VectorITransform(float const*, matrix3x4_t const&, float*)` | identical |  |
| mathlib_base | `SetIdentityMatrix(matrix3x4_t&)` | identical |  |
| mathlib_base | `_sqrtf(float)` | identical |  |
| matrix3 | `hk_Matrix3::rotate(int, float)` | identical |  |
| matrix3 | `hk_Matrix3::set_mul3_inv(hk_Rotation const&, hk_Matrix3 const&)` | identical |  |
| matrix3 | `hk_Matrix3::set_mul3(hk_Matrix3 const&, hk_Matrix3 const&)` | identical |  |
| memory | `hk_Memory::get_instance()` | differs | size retail 620 vs ours 571 |
| memory | `hk_Memory::deallocate(void*, int, hk_MEMORY_CLASS)` | identical |  |
| memory | `hk_Memory::allocate_real(int)` | identical |  |
| memory | `hk_Memory::allocate(int, hk_MEMORY_CLASS)` | identical |  |
| physics_airboat | `IVP_Controller::get_controller_name()` | identical |  |
| physics_airboat | `IVP_Controller::~IVP_Controller()` | identical |  |
| physics_airboat | `IVP_Controller::~IVP_Controller()` | identical |  |
| physics_airboat | `CPhysics_Airboat::do_steering(float, bool)` | identical |  |
| physics_airboat | `CPhysics_Airboat::PostRaycasts(IVP_Ray_Solver_Template*, IVP_U_Matrix const*, IVP_Raycast_Airboat_Impact*)` | identical |  |
| physics_airboat | `CPhysics_Airboat::update_throttle(float)` | identical |  |
| physics_airboat | `CPhysics_Airboat::DoSimulationDrag(IVP_Raycast_Airboat_Impact*, IVP_Event_Sim*)` | identical |  |
| physics_airboat | `CPhysics_Airboat::do_steering_wheel(IVP_POS_WHEEL, float)` | identical |  |
| physics_airboat | `CPhysics_Airboat::DoSimulationTurbine(IVP_Event_Sim*)` | identical |  |
| physics_airboat | `CPhysics_Airboat::change_wheel_torque(IVP_POS_WHEEL, float)` | identical |  |
| physics_airboat | `CPhysics_Airboat::get_controller_name()` | identical |  |
| physics_airboat | `CPhysics_Airboat::DoSimulationPontoons(IVP_Raycast_Airboat_Impact*, IVP_Event_Sim*)` | identical |  |
| physics_airboat | `CPhysics_Airboat::DoSimulationSteering(IVP_Event_Sim*)` | identical |  |
| physics_airboat | `CPhysics_Airboat::InitRaycastCarWheels(IVP_Template_Car_System const*)` | identical |  |
| physics_airboat | `CPhysics_Airboat::do_raycasts_gameside(int, IVP_Ray_Solver_Template*, IVP_Raycast_Airboat_Impact*)` | identical |  |
| physics_airboat | `CPhysics_Airboat::pre_raycasts_gameside(int, IVP_Ray_Solver_Template*, Ray_t*, IVP_Raycast_Airboat_Impact*)` | identical |  |
| physics_airboat | `CPhysics_Airboat::get_orig_axles_distance()` | identical |  |
| physics_airboat | `CPhysics_Airboat::do_simulation_controller(IVP_Event_Sim*, IVP_U_Vector<IVP_Core>*)` | identical |  |
| physics_airboat | `CPhysics_Airboat::DoSimulationPontoonsGround(IVP_Raycast_Airboat_Wheel*, IVP_Raycast_Airboat_Impact*, IVP_Event_Sim*)` | identical |  |
| physics_airboat | `CPhysics_Airboat::DoSimulationKeepUprightRoll(IVP_Raycast_Airboat_Impact*, IVP_Event_Sim*)` | identical |  |
| physics_airboat | `CPhysics_Airboat::DoSimulationKeepUprightPitch(IVP_Raycast_Airboat_Impact*, IVP_Event_Sim*)` | identical |  |
| physics_airboat | `CPhysics_Airboat::get_orig_front_wheel_distance()` | identical |  |
| physics_airboat | `CPhysics_Airboat::CPhysics_Airboat(IVP_Environment*, IVP_Template_Car_System const*, IPhysicsGameTrace*)` | edited |  |
| physics_airboat | `CPhysics_Airboat::~CPhysics_Airboat()` | identical |  |
| physics_airboat | `CPhysics_Airboat::~CPhysics_Airboat()` | identical |  |
| physics_airboat | `non-virtual thunk to CPhysics_Airboat::get_controller_name()` | identical |  |
| physics_airboat | `non-virtual thunk to CPhysics_Airboat::~CPhysics_Airboat()` | identical |  |
| physics_airboat | `__clang_call_terminate` | identical |  |
| physics_airboat | `IVP_Controller::reset_time(IVP_Time)` | identical |  |
| physics_airboat | `IVP_Controller::get_minimum_simulation_frequency()` | identical |  |
| physics_airboat | `IVP_Controller::core_is_going_to_be_deleted_event(IVP_Core*)` | identical |  |
| physics_airboat | `CPhysics_Airboat::IsBoosting()` | identical |  |
| physics_airboat | `CPhysics_Airboat::get_skid_info(IVP_Wheel_Skid_Info*)` | identical |  |
| physics_airboat | `CPhysics_Airboat::get_body_speed(IVP_COORDINATE_INDEX)` | identical |  |
| physics_airboat | `CPhysics_Airboat::set_powerslide(float, float)` | identical |  |
| physics_airboat | `CPhysics_Airboat::update_booster(float)` | identical |  |
| physics_airboat | `CPhysics_Airboat::SetWheelFriction(int, float)` | identical |  |
| physics_airboat | `CPhysics_Airboat::activate_booster(float, float, float)` | identical |  |
| physics_airboat | `CPhysics_Airboat::get_wheel_torque(IVP_POS_WHEEL)` | identical |  |
| physics_airboat | `CPhysics_Airboat::get_booster_delay()` | identical |  |
| physics_airboat | `CPhysics_Airboat::change_spring_length(IVP_POS_WHEEL, float)` | identical |  |
| physics_airboat | `CPhysics_Airboat::GetCarSystemDebugData(IVP_CarSystemDebugData_t&)` | identical |  |
| physics_airboat | `CPhysics_Airboat::SetCarSystemDebugData(IVP_CarSystemDebugData_t const&)` | identical |  |
| physics_airboat | `CPhysics_Airboat::change_body_downforce(float)` | identical |  |
| physics_airboat | `CPhysics_Airboat::change_max_body_force(IVP_POS_WHEEL, float)` | identical |  |
| physics_airboat | `CPhysics_Airboat::change_spring_constant(IVP_POS_WHEEL, float)` | identical |  |
| physics_airboat | `CPhysics_Airboat::get_booster_time_to_go()` | identical |  |
| physics_airboat | `CPhysics_Airboat::update_wheel_positions()` | identical |  |
| physics_airboat | `CPhysics_Airboat::change_fast_turn_factor(float)` | identical |  |
| physics_airboat | `CPhysics_Airboat::change_spring_dampening(IVP_POS_WHEEL, float)` | identical |  |
| physics_airboat | `CPhysics_Airboat::get_controller_priority()` | identical |  |
| physics_airboat | `CPhysics_Airboat::set_booster_acceleration(float)` | identical |  |
| physics_airboat | `CPhysics_Airboat::change_spring_pre_tension(IVP_POS_WHEEL, float)` | identical |  |
| physics_airboat | `CPhysics_Airboat::update_body_countertorque()` | identical |  |
| physics_airboat | `CPhysics_Airboat::change_stabilizer_constant(IVP_POS_AXIS, float)` | identical |  |
| physics_airboat | `CPhysics_Airboat::get_wheel_angular_velocity(IVP_POS_WHEEL)` | identical |  |
| physics_airboat | `CPhysics_Airboat::get_associated_controlled_cores()` | identical |  |
| physics_airboat | `CPhysics_Airboat::core_is_going_to_be_deleted_event(IVP_Core*)` | identical |  |
| physics_airboat | `CPhysics_Airboat::change_spring_dampening_compression(IVP_POS_WHEEL, float)` | identical |  |
| physics_airboat | `CPhysics_Airboat::GetWheel(int)` | identical |  |
| physics_airboat | `CPhysics_Airboat::fix_wheel(IVP_POS_WHEEL, IVP_BOOL)` | identical |  |
| physics_airboat | `CPhysics_Airboat::CPhysics_Airboat(IVP_Environment*, IVP_Template_Car_System const*, IPhysicsGameTrace*)` | identical |  |
| physics_airboat | `CPhysics_Airboat::~CPhysics_Airboat()` | identical |  |
| physics_airboat | `CAirboatFrictionData::GetContactPoint(Vector&)` | identical |  |
| physics_airboat | `CAirboatFrictionData::GetContactSpeed(Vector&)` | identical |  |
| physics_airboat | `CAirboatFrictionData::GetSurfaceNormal(Vector&)` | identical |  |
| physics_airboat | `non-virtual thunk to CPhysics_Airboat::get_controller_priority()` | identical |  |
| physics_airboat | `non-virtual thunk to CPhysics_Airboat::do_simulation_controller(IVP_Event_Sim*, IVP_U_Vector<IVP_Core>*)` | identical |  |
| physics_airboat | `non-virtual thunk to CPhysics_Airboat::get_associated_controlled_cores()` | identical |  |
| physics_airboat | `non-virtual thunk to CPhysics_Airboat::core_is_going_to_be_deleted_event(IVP_Core*)` | identical |  |
| physics_airboat | `non-virtual thunk to CPhysics_Airboat::~CPhysics_Airboat()` | identical |  |
| physics_collide | `_GLOBAL__I_a` | identical |  |
| physics_collide | `__CreateCPhysicsCollisionIPhysicsCollision_interface()` | identical |  |
| physics_collide | `CUtlMemory<bboxcache_t, int>::Grow(int)` | identical |  |
| physics_collide | `CPhysCollide::UnserializeFromBuffer(char const*, unsigned int, int, bool)` | identical |  |
| physics_collide | `CPhysPolysoup::CPhysPolysoup()` | identical |  |
| physics_collide | `CCollisionQuery::GetTriangleVerts(int, int, Vector*)` | identical |  |
| physics_collide | `CCollisionQuery::~CCollisionQuery()` | identical |  |
| physics_collide | `CCollisionQuery::~CCollisionQuery()` | identical |  |
| physics_collide | `CPhysicsCollision::ConvexFree(CPhysConvex*)` | identical |  |
| physics_collide | `CPhysicsCollision::AddBBoxCache(CPhysCollideCompactSurface*, Vector const&, Vector const&)` | identical |  |
| physics_collide | `CPhysicsCollision::BBoxToConvex(Vector const&, Vector const&)` | identical |  |
| physics_collide | `CPhysicsCollision::ConvexVolume(CPhysConvex*)` | identical |  |
| physics_collide | `CPhysicsCollision::TraceCollide(Vector const&, Vector const&, CPhysCollide const*, QAngle const&, CPhysCollide const*, Vector const&, QAngle const&, C` | identical |  |
| physics_collide | `CPhysicsCollision::VCollideLoad(vcollide_t*, int, char const*, int, bool)` | identical |  |
| physics_collide | `CPhysicsCollision::CollideVolume(CPhysCollide*)` | identical |  |
| physics_collide | `CPhysicsCollision::InitBBoxCache()` | identical |  |
| physics_collide | `CPhysicsCollision::CollideGetAABB(Vector*, Vector*, CPhysCollide const*, Vector const&, QAngle const&)` | identical |  |
| physics_collide | `CPhysicsCollision::PolysoupCreate()` | identical |  |
| physics_collide | `CPhysicsCollision::VCollideUnload(vcollide_t*)` | identical |  |
| physics_collide | `CPhysicsCollision::CreateDebugMesh(CPhysCollide const*, Vector**)` | identical |  |
| physics_collide | `CPhysicsCollision::FastBboxCollide(CPhysCollideCompactSurface const*, Vector const&, Vector const&)` | identical |  |
| physics_collide | `CPhysicsCollision::PolysoupDestroy(CPhysPolysoup*)` | identical |  |
| physics_collide | `CPhysicsCollision::CollideGetExtent(CPhysCollide const*, Vector const&, QAngle const&, Vector const&)` | identical |  |
| physics_collide | `CPhysicsCollision::ConvexFromPlanes(float*, int, float)` | edited |  |
| physics_collide | `CPhysicsCollision::CreateQueryModel(CPhysCollide*)` | identical |  |
| physics_collide | `CPhysicsCollision::DestroyDebugMesh(int, Vector*)` | identical |  |
| physics_collide | `CPhysicsCollision::ConvexSurfaceArea(CPhysConvex*)` | identical |  |
| physics_collide | `CPhysicsCollision::CreateVirtualMesh(virtualmeshparams_t const&)` | identical |  |
| physics_collide | `CPhysicsCollision::CollideSurfaceArea(CPhysCollide*)` | identical |  |
| physics_collide | `CPhysicsCollision::ConvexFromVertsFast(Vector**, int)` | identical |  |
| physics_collide | `CPhysicsCollision::PolysoupAddTriangle(CPhysPolysoup*, Vector const&, Vector const&, Vector const&, int)` | identical |  |
| physics_collide | `CPhysicsCollision::PolyhedronFromConvex(CPhysConvex*, bool)` | identical |  |
| physics_collide | `CPhysicsCollision::IsBoxIntersectingCone(Vector const&, Vector const&, truncatedcone_t const&)` | identical |  |
| physics_collide | `CPhysicsCollision::RebuildConvexFromPlanes(CPhysConvex*, float)` | edited |  |
| physics_collide | `CPhysicsCollision::VPhysicsKeyParserCreate(char const*)` | identical |  |
| physics_collide | `CPhysicsCollision::ConvertPolysoupToCollide(CPhysPolysoup*, bool)` | identical |  |
| physics_collide | `CPhysicsCollision::VPhysicsKeyParserDestroy(IVPhysicsKeyParser*)` | identical |  |
| physics_collide | `CPhysicsCollision::ConvexesFromConvexPolygon(Vector const&, Vector const*, int, CPhysConvex**)` | identical |  |
| physics_collide | `CPhysicsCollision::ConvexFromConvexPolyhedron(CPolyhedron const&)` | identical |  |
| physics_collide | `CPhysicsCollision::CollideGetOrthographicAreas(CPhysCollide const*)` | identical |  |
| physics_collide | `CPhysicsCollision::ConvertConvexToCollideParams(CPhysConvex**, int, convertconvexparams_t const&)` | identical |  |
| physics_collide | `CPhysicsCollision::GetConvexesUsedInCollideable(CPhysCollide const*, CPhysConvex**, int)` | identical |  |
| physics_collide | `CPhysicsCollision::TraceBox(Ray_t const&, unsigned int, IConvexInfo*, CPhysCollide const*, Vector const&, QAngle const&, CGameTrace*)` | identical |  |
| physics_collide | `CPhysicsCollision::TraceBox(Vector const&, Vector const&, Vector const&, Vector const&, CPhysCollide const*, Vector const&, QAngle const&, CGameTrace*` | identical |  |
| physics_collide | `CPhysicsCollision::~CPhysicsCollision()` | identical |  |
| physics_collide | `CPhysicsCollision::~CPhysicsCollision()` | identical |  |
| physics_collide | `CPhysCollideCompactSurface::SetMassCenter(Vector const&)` | identical |  |
| physics_collide | `CPhysCollideCompactSurface::InitCollideMap()` | identical |  |
| physics_collide | `CPhysCollideCompactSurface::ComputeOrthographicAreas(float)` | identical |  |
| physics_collide | `CPhysCollideCompactSurface::~CPhysCollideCompactSurface()` | identical |  |
| physics_collide | `CPhysCollideCompactSurface::~CPhysCollideCompactSurface()` | identical |  |
| physics_collide | `CDatadescGeneratedNameHolder::~CDatadescGeneratedNameHolder()` | identical |  |
| physics_collide | `CPhysCollideCompactSurface::GetAllLedges(IVP_U_BigVector<IVP_Compact_Ledge>&) const` | identical |  |
| physics_collide | `CPhysCollideCompactSurface::GetMassCenter() const` | identical |  |
| physics_collide | `CPhysCollideCompactSurface::OutputDebugInfo() const` | identical |  |
| physics_collide | `CPhysCollideCompactSurface::SerializeToBuffer(char*, bool) const` | identical |  |
| physics_collide | `CPhysCollideCompactSurface::CreateSurfaceManager(short&) const` | identical |  |
| physics_collide | `CreateSurfaceManager(CPhysCollide const*, short&)` | identical |  |
| physics_collide | `OutputCollideDebugInfo(CPhysCollide const*)` | identical |  |
| physics_collide | `CCollisionQuery::ConvexCount()` | identical |  |
| physics_collide | `CCollisionQuery::GetGameData(int)` | identical |  |
| physics_collide | `CCollisionQuery::TriangleCount(int)` | identical |  |
| physics_collide | `CCollisionQuery::SetTriangleVerts(int, int, Vector const*)` | identical |  |
| physics_collide | `CCollisionQuery::GetTriangleMaterialIndex(int, int)` | identical |  |
| physics_collide | `CCollisionQuery::SetTriangleMaterialIndex(int, int, int)` | identical |  |
| physics_collide | `CPhysicsCollision::CollideSize(CPhysCollide*)` | identical |  |
| physics_collide | `CPhysicsCollision::CollideIndex(CPhysCollide const*)` | identical |  |
| physics_collide | `CPhysicsCollision::CollideWrite(char*, CPhysCollide*, bool)` | identical |  |
| physics_collide | `CPhysicsCollision::BBoxToCollide(Vector const&, Vector const&)` | identical |  |
| physics_collide | `CPhysicsCollision::DestroyCollide(CPhysCollide*)` | identical |  |
| physics_collide | `CPhysicsCollision::ConvexFromVerts(Vector**, int)` | identical |  |
| physics_collide | `CPhysicsCollision::OutputDebugInfo(CPhysCollide const*)` | identical |  |
| physics_collide | `CPhysicsCollision::GetBBoxCacheSize(int*, int*)` | identical |  |
| physics_collide | `CPhysicsCollision::DestroyQueryModel(ICollisionQuery*)` | identical |  |
| physics_collide | `CPhysicsCollision::SetConvexGameData(CPhysConvex*, unsigned int)` | identical |  |
| physics_collide | `CPhysicsCollision::UnserializeCollide(char*, int, int)` | identical |  |
| physics_collide | `CPhysicsCollision::SupportsVirtualMesh()` | identical |  |
| physics_collide | `CPhysicsCollision::ThreadContextCreate()` | identical |  |
| physics_collide | `CPhysicsCollision::CollideGetMassCenter(CPhysCollide*, Vector*)` | identical |  |
| physics_collide | `CPhysicsCollision::CollideSetMassCenter(CPhysCollide*, Vector const&)` | identical |  |
| physics_collide | `CPhysicsCollision::ThreadContextDestroy(IPhysicsCollision*)` | identical |  |
| physics_collide | `CPhysicsCollision::ConvertConvexToCollide(CPhysConvex**, int)` | identical |  |
| physics_collide | `CPhysicsCollision::CollideSetOrthographicAreas(CPhysCollide*, Vector const&)` | identical |  |
| physics_collide | `CPhysicsCollision::ReadStat(int)` | identical |  |
| physics_collide | `CPhysicsCollision::TraceBox(Ray_t const&, CPhysCollide const*, Vector const&, QAngle const&, CGameTrace*)` | identical |  |
| physics_collide | `CPhysCollideCompactSurface::SetOrthographicAreas(Vector const&)` | identical |  |
| physics_collide | `CPhysCollide::GetSphereRadius() const` | identical |  |
| physics_collide | `CPhysCollideCompactSurface::GetCollideMap() const` | identical |  |
| physics_collide | `CPhysCollideCompactSurface::GetVCollideIndex() const` | identical |  |
| physics_collide | `CPhysCollideCompactSurface::ComputeHullInfo_r(CPhysCollideCompactSurface::hullinfo_t*, IVP_Compact_Ledgetree_Node const*) const` | identical |  |
| physics_collide | `CPhysCollideCompactSurface::GetCompactSurface() const` | identical |  |
| physics_collide | `CPhysCollideCompactSurface::GetOrthographicAreas() const` | identical |  |
| physics_collide | `CPhysCollideCompactSurface::GetSerializationSize() const` | identical |  |
| physics_constraint | `CPhysicsConstraint::WriteLength(constraint_lengthparams_t&) const` | differs | size retail 598 vs ours 591 |
| physics_constraint | `CPhysicsConstraint::WriteRagdoll(constraint_ragdollparams_t&) const` | differs | bytes |
| physics_constraint | `CPhysicsConstraint::GetConstraintTransform(matrix3x4_t*, matrix3x4_t*) const` | differs | size retail 1185 vs ours 1533 |
| physics_constraint | `_GLOBAL__I_a` | identical |  |
| physics_constraint | `SeedRandomGenerators()` | identical |  |
| physics_constraint | `CreateFixedConstraint(IVP_Environment*, CPhysicsObject*, CPhysicsObject*, IPhysicsConstraintGroup*, constraint_fixedparams_t const&)` | identical |  |
| physics_constraint | `CreateHingeConstraint(IVP_Environment*, CPhysicsObject*, CPhysicsObject*, IPhysicsConstraintGroup*, constraint_limitedhingeparams_t const&)` | identical |  |
| physics_constraint | `SavePhysicsConstraint(physsaveparams_t const&, CPhysicsConstraint*)` | identical |  |
| physics_constraint | `CreateLengthConstraint(IVP_Environment*, CPhysicsObject*, CPhysicsObject*, IPhysicsConstraintGroup*, constraint_lengthparams_t const&)` | identical |  |
| physics_constraint | `CreatePulleyConstraint(IVP_Environment*, CPhysicsObject*, CPhysicsObject*, IPhysicsConstraintGroup*, constraint_pulleyparams_t const&)` | identical |  |
| physics_constraint | `CreateRagdollConstraint(IVP_Environment*, CPhysicsObject*, CPhysicsObject*, IPhysicsConstraintGroup*, constraint_ragdollparams_t const&)` | identical |  |
| physics_constraint | `CreateSlidingConstraint(IVP_Environment*, CPhysicsObject*, CPhysicsObject*, IPhysicsConstraintGroup*, constraint_slidingparams_t const&)` | identical |  |
| physics_constraint | `RestorePhysicsConstraint(physrestoreparams_t const&, CPhysicsConstraint**)` | identical |  |
| physics_constraint | `CreateBallsocketConstraint(IVP_Environment*, CPhysicsObject*, CPhysicsObject*, IPhysicsConstraintGroup*, constraint_ballsocketparams_t const&)` | identical |  |
| physics_constraint | `SavePhysicsConstraintGroup(physsaveparams_t const&, CPhysicsConstraintGroup*)` | edited |  |
| physics_constraint | `CreatePhysicsConstraintGroup(IVP_Environment*, constraint_groupparams_t const&)` | edited |  |
| physics_constraint | `RestorePhysicsConstraintGroup(physrestoreparams_t const&, CPhysicsConstraintGroup**)` | identical |  |
| physics_constraint | `PostRestorePhysicsConstraintGroup()` | identical |  |
| physics_constraint | `CUtlMemory<CPhysicsConstraintGroup*, int>::Grow(int)` | identical |  |
| physics_constraint | `CUtlVector<CPhysicsConstraintGroup*, CUtlMemory<CPhysicsConstraintGroup*, int>>::~CUtlVector()` | identical |  |
| physics_constraint | `CPhysicsConstraint::Deactivate()` | identical |  |
| physics_constraint | `CPhysicsConstraint::InitLength(IVP_Environment*, CPhysicsConstraintGroup*, constraint_lengthparams_t const&)` | edited |  |
| physics_constraint | `CPhysicsConstraint::InitPulley(IVP_Environment*, CPhysicsConstraintGroup*, constraint_pulleyparams_t const&)` | edited |  |
| physics_constraint | `CPhysicsConstraint::InitRagdoll(IVP_Environment*, CPhysicsConstraintGroup*, constraint_ragdollparams_t const&)` | edited |  |
| physics_constraint | `CPhysicsConstraint::InitSliding(IVP_Environment*, CPhysicsConstraintGroup*, constraint_slidingparams_t const&)` | edited |  |
| physics_constraint | `CPhysicsConstraint::InitBallsocket(IVP_Environment*, CPhysicsConstraintGroup*, constraint_ballsocketparams_t const&)` | edited |  |
| physics_constraint | `CPhysicsConstraint::SetLinearMotor(float, float)` | edited |  |
| physics_constraint | `CPhysicsConstraint::OutputDebugInfo()` | identical |  |
| physics_constraint | `CPhysicsConstraint::SetAngularMotor(float, float)` | edited |  |
| physics_constraint | `CPhysicsConstraint::event_object_deleted(IVP_Event_Object*)` | identical |  |
| physics_constraint | `CPhysicsConstraint::UpdateRagdollTransforms(matrix3x4_t const&, matrix3x4_t const&)` | edited |  |
| physics_constraint | `CPhysicsConstraint::CreateBreakableConstraint(hk_Constraint*, hk_Local_Constraint_System*, constraint_breakableparams_t const&)` | new |  |
| physics_constraint | `CPhysicsConstraint::Activate()` | identical |  |
| physics_constraint | `CPhysicsConstraint::InitFixed(IVP_Environment*, CPhysicsConstraintGroup*, constraint_fixedparams_t const&)` | edited |  |
| physics_constraint | `CPhysicsConstraint::InitHinge(IVP_Environment*, CPhysicsConstraintGroup*, constraint_limitedhingeparams_t const&)` | edited |  |
| physics_constraint | `CPhysicsConstraint::~CPhysicsConstraint()` | identical |  |
| physics_constraint | `CPhysicsConstraint::~CPhysicsConstraint()` | identical |  |
| physics_constraint | `CPhysicsConstraintGroup::GetErrorParams(constraint_groupparams_t*)` | edited |  |
| physics_constraint | `CPhysicsConstraintGroup::SetErrorParams(constraint_groupparams_t const&)` | identical |  |
| physics_constraint | `CPhysicsConstraintGroup::SolvePenetration(IPhysicsObject*, IPhysicsObject*)` | identical |  |
| physics_constraint | `CPhysicsConstraintGroup::Activate()` | identical |  |
| physics_constraint | `CPhysicsConstraintGroup::~CPhysicsConstraintGroup()` | identical |  |
| physics_constraint | `CPhysicsConstraintGroup::~CPhysicsConstraintGroup()` | identical |  |
| physics_constraint | `CPhysicsConstraint::WriteFixed(constraint_fixedparams_t&) const` | edited |  |
| physics_constraint | `CPhysicsConstraint::WriteHinge(constraint_hingeparams_t&) const` | edited |  |
| physics_constraint | `CPhysicsConstraint::WritePulley(constraint_pulleyparams_t&) const` | edited |  |
| physics_constraint | `CPhysicsConstraint::WriteSliding(constraint_slidingparams_t&) const` | edited |  |
| physics_constraint | `CPhysicsConstraint::WriteBallsocket(constraint_ballsocketparams_t&) const` | edited |  |
| physics_constraint | `CPhysicsConstraint::WriteToTemplate(vphysics_save_cphysicsconstraint_t&, vphysics_save_constraint_t&) const` | identical |  |
| physics_constraint | `CPhysicsConstraint::GetConstraintParams(constraint_breakableparams_t*) const` | identical |  |
| physics_constraint | `non-virtual thunk to CPhysicsConstraint::~CPhysicsConstraint()` | identical |  |
| physics_constraint | `IsExternalConstraint(IVP_Controller*, void*)` | identical |  |
| physics_constraint | `GetClientDataForHkConstraint(hk_Breakable_Constraint*)` | identical |  |
| physics_constraint | `CPhysicsConstraint::SetGameData(void*)` | identical |  |
| physics_constraint | `CPhysicsConstraint::event_object_frozen(IVP_Event_Object*)` | identical |  |
| physics_constraint | `CPhysicsConstraint::event_object_created(IVP_Event_Object*)` | identical |  |
| physics_constraint | `CPhysicsConstraint::event_object_revived(IVP_Event_Object*)` | identical |  |
| physics_constraint | `CPhysicsConstraint::~CPhysicsConstraint()` | identical |  |
| physics_constraint | `CPhysicsConstraintGroup::IsInErrorState()` | identical |  |
| physics_constraint | `CPhysicsConstraintGroup::ClearErrorState()` | identical |  |
| physics_constraint | `CPhysicsConstraint::GetGameData() const` | identical |  |
| physics_constraint | `CPhysicsConstraint::GetAttachedObject() const` | identical |  |
| physics_constraint | `CPhysicsConstraint::GetReferenceObject() const` | identical |  |
| physics_constraint | `non-virtual thunk to CPhysicsConstraint::event_object_frozen(IVP_Event_Object*)` | identical |  |
| physics_constraint | `non-virtual thunk to CPhysicsConstraint::event_object_created(IVP_Event_Object*)` | identical |  |
| physics_constraint | `non-virtual thunk to CPhysicsConstraint::event_object_deleted(IVP_Event_Object*)` | identical |  |
| physics_constraint | `non-virtual thunk to CPhysicsConstraint::event_object_revived(IVP_Event_Object*)` | identical |  |
| physics_constraint | `non-virtual thunk to CPhysicsConstraint::~CPhysicsConstraint()` | identical |  |
| physics_controller_raycast_vehicle | `IVP_Ray_Hit_Listener::~IVP_Ray_Hit_Listener()` | identical |  |
| physics_controller_raycast_vehicle | `IVP_Ray_Hit_Listener::~IVP_Ray_Hit_Listener()` | identical |  |
| physics_controller_raycast_vehicle | `CPhysics_Car_System_Raycast_Wheels::do_raycasts(IVP_Event_Sim*, int, IVP_Ray_Solver_Template*, IVP_Ray_Hit*, float*)` | identical |  |
| physics_controller_raycast_vehicle | `CPhysics_Car_System_Raycast_Wheels::get_controller_name()` | identical |  |
| physics_controller_raycast_vehicle | `CPhysics_Car_System_Raycast_Wheels::update_wheel_positions()` | identical |  |
| physics_controller_raycast_vehicle | `CPhysics_Car_System_Raycast_Wheels::CPhysics_Car_System_Raycast_Wheels(IVP_Environment*, IVP_Template_Car_System const*)` | identical |  |
| physics_controller_raycast_vehicle | `CPhysics_Car_System_Raycast_Wheels::~CPhysics_Car_System_Raycast_Wheels()` | identical |  |
| physics_controller_raycast_vehicle | `CPhysics_Car_System_Raycast_Wheels::~CPhysics_Car_System_Raycast_Wheels()` | identical |  |
| physics_controller_raycast_vehicle | `non-virtual thunk to CPhysics_Car_System_Raycast_Wheels::get_controller_name()` | identical |  |
| physics_controller_raycast_vehicle | `non-virtual thunk to CPhysics_Car_System_Raycast_Wheels::~CPhysics_Car_System_Raycast_Wheels()` | identical |  |
| physics_controller_raycast_vehicle | `non-virtual thunk to CPhysics_Car_System_Raycast_Wheels::~CPhysics_Car_System_Raycast_Wheels()` | identical |  |
| physics_controller_raycast_vehicle | `IVP_Controller_Raycast_Car::set_powerslide(float, float)` | identical |  |
| physics_controller_raycast_vehicle | `IVP_Controller_Raycast_Car::update_booster(float)` | identical |  |
| physics_controller_raycast_vehicle | `IVP_Controller_Raycast_Car::update_throttle(float)` | identical |  |
| physics_controller_raycast_vehicle | `IVP_Controller_Raycast_Car::change_max_body_force(IVP_POS_WHEEL, float)` | identical |  |
| physics_controller_raycast_vehicle | `IVP_Controller_Raycast_Car::get_booster_time_to_go()` | identical |  |
| physics_controller_raycast_vehicle | `IVP_Controller_Raycast_Car::update_body_countertorque()` | identical |  |
| physics_controller_raycast_vehicle | `IVP_Controller_Raycast_Car::get_associated_controlled_cores()` | identical |  |
| physics_controller_raycast_vehicle | `IVP_Controller_Raycast_Car::core_is_going_to_be_deleted_event(IVP_Core*)` | identical |  |
| physics_controller_raycast_vehicle | `CPhysics_Car_System_Raycast_Wheels::GetWheel(int)` | identical |  |
| physics_controller_raycast_vehicle | `non-virtual thunk to IVP_Controller_Raycast_Car::get_associated_controlled_cores()` | identical |  |
| physics_controller_raycast_vehicle | `non-virtual thunk to IVP_Controller_Raycast_Car::core_is_going_to_be_deleted_event(IVP_Core*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::CPhysicsEnvironment()` | differs | bytes |
| physics_environment | `CreateObjectPairHash()` | identical |  |
| physics_environment | `CreatePhysicsEnvironment()` | identical |  |
| physics_environment | `CUtlMemory<UtlRBTreeNode_t<CPhysicsListenerCollision::corepair_t, unsigned short>, unsigned short>::Grow(int)` | identical |  |
| physics_environment | `CUtlMemory<UtlLinkedListElem_t<CUtlMultiList<void*, unsigned short>::List_t, unsigned short>, unsigned short>::Grow(int)` | identical |  |
| physics_environment | `CUtlMemory<CUtlMultiList<void*, unsigned short>::ListElem_t, int>::Grow(int)` | identical |  |
| physics_environment | `CUtlMemory<CCollisionSolver::realobjectpair_t, int>::Grow(int)` | identical |  |
| physics_environment | `CUtlMemory<CPhysicsObject*, int>::Grow(int)` | identical |  |
| physics_environment | `CUtlMemory<IPhysicsObject*, int>::Grow(int)` | identical |  |
| physics_environment | `CUtlMemory<IDeleteQueueItem*, int>::Grow(int)` | identical |  |
| physics_environment | `CUtlMemory<CPhysicsFluidController*, int>::Grow(int)` | identical |  |
| physics_environment | `CUtlMemory<IPhysicsPlayerController*, int>::Grow(int)` | identical |  |
| physics_environment | `CUtlRBTree<CPhysicsListenerCollision::corepair_t, unsigned short, bool (*)(CPhysicsListenerCollision::corepair_t const&, CPhysicsListenerCollision::co` | identical |  |
| physics_environment | `CUtlRBTree<CPhysicsListenerCollision::corepair_t, unsigned short, bool (*)(CPhysicsListenerCollision::corepair_t const&, CPhysicsListenerCollision::co` | identical |  |
| physics_environment | `CUtlRBTree<CPhysicsListenerCollision::corepair_t, unsigned short, bool (*)(CPhysicsListenerCollision::corepair_t const&, CPhysicsListenerCollision::co` | identical |  |
| physics_environment | `CUtlRBTree<CPhysicsListenerCollision::corepair_t, unsigned short, bool (*)(CPhysicsListenerCollision::corepair_t const&, CPhysicsListenerCollision::co` | identical |  |
| physics_environment | `CUtlRBTree<CPhysicsListenerCollision::corepair_t, unsigned short, bool (*)(CPhysicsListenerCollision::corepair_t const&, CPhysicsListenerCollision::co` | identical |  |
| physics_environment | `CUtlRBTree<CPhysicsListenerCollision::corepair_t, unsigned short, bool (*)(CPhysicsListenerCollision::corepair_t const&, CPhysicsListenerCollision::co` | identical |  |
| physics_environment | `CUtlRBTree<CPhysicsListenerCollision::corepair_t, unsigned short, bool (*)(CPhysicsListenerCollision::corepair_t const&, CPhysicsListenerCollision::co` | identical |  |
| physics_environment | `CUtlRBTree<CPhysicsListenerCollision::corepair_t, unsigned short, bool (*)(CPhysicsListenerCollision::corepair_t const&, CPhysicsListenerCollision::co` | identical |  |
| physics_environment | `CDeleteProxy<IPhysicsConstraint>::~CDeleteProxy()` | identical |  |
| physics_environment | `CSleepObjects::UpdateSleepObjects()` | identical |  |
| physics_environment | `CSleepObjects::ProcessActiveObjects(IVP_Environment*, IPhysicsCollisionEvent*)` | identical |  |
| physics_environment | `CSleepObjects::event_object_revived(IVP_Event_Object*)` | identical |  |
| physics_environment | `CSleepObjects::~CSleepObjects()` | identical |  |
| physics_environment | `CSleepObjects::~CSleepObjects()` | identical |  |
| physics_environment | `CUtlMultiList<void*, unsigned short>::Alloc()` | identical |  |
| physics_environment | `CUtlMultiList<void*, unsigned short>::~CUtlMultiList()` | identical |  |
| physics_environment | `CVoidPairHash::~CVoidPairHash()` | identical |  |
| physics_environment | `CVoidPairHash::~CVoidPairHash()` | identical |  |
| physics_environment | `CUtlLinkedList<CUtlMultiList<void*, unsigned short>::List_t, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CUtlMultiList<void*` | identical |  |
| physics_environment | `CDragController::get_controller_name()` | identical |  |
| physics_environment | `CDragController::do_simulation_controller(IVP_Event_Sim*, IVP_U_Vector<IVP_Core>*)` | identical |  |
| physics_environment | `CDragController::~CDragController()` | identical |  |
| physics_environment | `CDragController::~CDragController()` | identical |  |
| physics_environment | `CObjectPairHash::AddObjectPair(void*, void*)` | identical |  |
| physics_environment | `CObjectPairHash::IsObjectInHash(void*)` | identical |  |
| physics_environment | `CObjectPairHash::AddToObjectList(void*, void*)` | identical |  |
| physics_environment | `CObjectPairHash::RemoveObjectPair(void*, void*)` | identical |  |
| physics_environment | `CObjectPairHash::DestroyListForObject(void*, unsigned short)` | identical |  |
| physics_environment | `CObjectPairHash::GetPairListForObject(void*, int, void**)` | identical |  |
| physics_environment | `CObjectPairHash::RemoveFromObjectList(void*, void*)` | identical |  |
| physics_environment | `CObjectPairHash::GetPairCountForObject(void*)` | identical |  |
| physics_environment | `CObjectPairHash::RemoveAllPairsForObject(void*)` | identical |  |
| physics_environment | `CObjectPairHash::CObjectPairHash()` | identical |  |
| physics_environment | `CObjectPairHash::~CObjectPairHash()` | identical |  |
| physics_environment | `CObjectPairHash::~CObjectPairHash()` | identical |  |
| physics_environment | `CCollisionSolver::CheckObjPair(IVP_Real_Object*, IVP_Real_Object*)` | identical |  |
| physics_environment | `CCollisionSolver::inter_penetration(IVP_Mindist*, IVP_Real_Object*, IVP_Real_Object*, double)` | identical |  |
| physics_environment | `CCollisionSolver::max_velocity_exceeded(IVP_Anomaly_Limits*, IVP_Core*, IVP_U_Float_Point*)` | identical |  |
| physics_environment | `CCollisionSolver::max_contacts_exceeded_check_freezing(IVP_Core**, int)` | identical |  |
| physics_environment | `CCollisionSolver::~CCollisionSolver()` | identical |  |
| physics_environment | `CCollisionSolver::~CCollisionSolver()` | identical |  |
| physics_environment | `CPhysicsEnvironment::ClearStats()` | identical |  |
| physics_environment | `CPhysicsEnvironment::PhantomAdd(CPhysicsObject*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::SetGravity(Vector const&)` | identical |  |
| physics_environment | `CPhysicsEnvironment::CreateSpring(IPhysicsObject*, IPhysicsObject*, springparams_t*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::DestroyObject(IPhysicsObject*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::TransferObject(IPhysicsObject*, IPhysicsEnvironment*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::SetDebugOverlay(void* (*)(char const*, int*))` | identical |  |
| physics_environment | `CPhysicsEnvironment::ClearDeadObjects()` | identical |  |
| physics_environment | `CPhysicsEnvironment::CreatePolyObject(CPhysCollide const*, int, Vector const&, QAngle const&, objectparams_t*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::DestroyConstraint(IPhysicsConstraint*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::CreateSphereObject(float, int, Vector const&, QAngle const&, objectparams_t*, bool)` | identical |  |
| physics_environment | `CPhysicsEnvironment::DebugCheckContacts()` | identical |  |
| physics_environment | `CPhysicsEnvironment::ResetSimulationClock()` | identical |  |
| physics_environment | `CPhysicsEnvironment::CreateFluidController(IPhysicsObject*, fluidparams_t*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::SetSimulationTimestep(float)` | identical |  |
| physics_environment | `CPhysicsEnvironment::CreatePlayerController(IPhysicsObject*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::CreatePolyObjectStatic(CPhysCollide const*, int, Vector const&, QAngle const&, objectparams_t*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::CreateShadowController(IPhysicsObject*, bool, bool)` | identical |  |
| physics_environment | `CPhysicsEnvironment::DestroyFluidController(IPhysicsFluidController*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::SetPerformanceSettings(physics_performanceparams_t const*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::CreateVehicleController(IPhysicsObject*, vehicleparams_t const&, unsigned int, IPhysicsGameTrace*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::DestroyPlayerController(IPhysicsPlayerController*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::UnserializeObjectFromBuffer(void*, unsigned char*, unsigned int, bool)` | identical |  |
| physics_environment | `CPhysicsEnvironment::Simulate(float)` | identical |  |
| physics_environment | `CPhysicsEnvironment::ReadStats(physics_stats_t*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::~CPhysicsEnvironment()` | identical |  |
| physics_environment | `CPhysicsEnvironment::~CPhysicsEnvironment()` | identical |  |
| physics_environment | `CPhysicsFrictionData::GetContactPoint(Vector&)` | identical |  |
| physics_environment | `CPhysicsFrictionData::GetContactSpeed(Vector&)` | identical |  |
| physics_environment | `CPhysicsFrictionData::GetSurfaceNormal(Vector&)` | identical |  |
| physics_environment | `CPhysicsCollisionData::GetContactPoint(Vector&)` | identical |  |
| physics_environment | `CPhysicsCollisionData::GetContactSpeed(Vector&)` | identical |  |
| physics_environment | `CPhysicsCollisionData::GetSurfaceNormal(Vector&)` | identical |  |
| physics_environment | `CPhysicsListenerCollision::UpdatePairListPSI(CPhysicsEnvironment*)` | identical |  |
| physics_environment | `CPhysicsListenerCollision::event_pre_collision(IVP_Event_Collision*)` | identical |  |
| physics_environment | `CPhysicsListenerCollision::event_post_collision(IVP_Event_Collision*)` | identical |  |
| physics_environment | `CPhysicsListenerCollision::event_friction_created(IVP_Event_Friction*)` | identical |  |
| physics_environment | `CPhysicsListenerCollision::event_friction_deleted(IVP_Event_Friction*)` | identical |  |
| physics_environment | `CPhysicsListenerCollision::event_friction_pair_created(IVP_Friction_Core_Pair*)` | identical |  |
| physics_environment | `CPhysicsListenerCollision::event_friction_pair_deleted(IVP_Friction_Core_Pair*)` | identical |  |
| physics_environment | `CPhysicsListenerCollision::~CPhysicsListenerCollision()` | identical |  |
| physics_environment | `CPhysicsListenerCollision::~CPhysicsListenerCollision()` | identical |  |
| physics_environment | `IVP_Controller_Independent::get_associated_controlled_cores()` | identical |  |
| physics_environment | `CUtlRBTree<CPhysicsListenerCollision::corepair_t, unsigned short, bool (*)(CPhysicsListenerCollision::corepair_t const&, CPhysicsListenerCollision::co` | identical |  |
| physics_environment | `CUtlRBTree<CPhysicsListenerCollision::corepair_t, unsigned short, bool (*)(CPhysicsListenerCollision::corepair_t const&, CPhysicsListenerCollision::co` | identical |  |
| physics_environment | `CUtlRBTree<CPhysicsListenerCollision::corepair_t, unsigned short, bool (*)(CPhysicsListenerCollision::corepair_t const&, CPhysicsListenerCollision::co` | identical |  |
| physics_environment | `CUtlRBTree<CPhysicsListenerCollision::corepair_t, unsigned short, bool (*)(CPhysicsListenerCollision::corepair_t const&, CPhysicsListenerCollision::co` | identical |  |
| physics_environment | `CUtlRBTree<CPhysicsListenerCollision::corepair_t, unsigned short, bool (*)(CPhysicsListenerCollision::corepair_t const&, CPhysicsListenerCollision::co` | identical |  |
| physics_environment | `CPhysicsEnvironment::GetGravity(Vector*) const` | identical |  |
| physics_environment | `CPhysicsEnvironment::GetPerformanceSettings(physics_performanceparams_t*) const` | identical |  |
| physics_environment | `non-virtual thunk to CCollisionSolver::max_velocity_exceeded(IVP_Anomaly_Limits*, IVP_Core*, IVP_U_Float_Point*)` | identical |  |
| physics_environment | `non-virtual thunk to CCollisionSolver::~CCollisionSolver()` | identical |  |
| physics_environment | `non-virtual thunk to CCollisionSolver::~CCollisionSolver()` | identical |  |
| physics_environment | `GetOppositeSynapseObject(IVP_Synapse_Friction*)` | identical |  |
| physics_environment | `CUtlRBTree<CPhysicsListenerCollision::corepair_t, unsigned short, bool (*)(CPhysicsListenerCollision::corepair_t const&, CPhysicsListenerCollision::co` | identical |  |
| physics_environment | `CDeleteProxy<IPhysicsConstraint>::Delete()` | identical |  |
| physics_environment | `CDeleteProxy<IPhysicsConstraint>::~CDeleteProxy()` | identical |  |
| physics_environment | `CSleepObjects::event_object_frozen(IVP_Event_Object*)` | identical |  |
| physics_environment | `CSleepObjects::event_object_created(IVP_Event_Object*)` | identical |  |
| physics_environment | `CSleepObjects::event_object_deleted(IVP_Event_Object*)` | identical |  |
| physics_environment | `CUtlMultiList<void*, unsigned short>::LinkBefore(unsigned short, unsigned short, unsigned short)` | identical |  |
| physics_environment | `CUtlMultiList<void*, unsigned short>::Remove(unsigned short, unsigned short)` | identical |  |
| physics_environment | `CUtlLinkedList<CUtlMultiList<void*, unsigned short>::List_t, unsigned short, false, unsigned short, CUtlMemory<UtlLinkedListElem_t<CUtlMultiList<void*` | identical |  |
| physics_environment | `CDragController::get_controller_priority()` | identical |  |
| physics_environment | `CObjectPairHash::IsObjectPairInHash(void*, void*)` | identical |  |
| physics_environment | `CCollisionSolver::environment_will_be_deleted(IVP_Environment*)` | identical |  |
| physics_environment | `CCollisionSolver::max_collision_checks_exceeded(int)` | identical |  |
| physics_environment | `CCollisionSolver::check_objects_for_collision_detection(IVP_Real_Object*, IVP_Real_Object*)` | identical |  |
| physics_environment | `CCollisionSolver::max_collisions_exceeded_check_freezing(IVP_Anomaly_Limits*, IVP_Core*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::DestroySpring(IPhysicsSpring*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::SetAirDensity(float)` | identical |  |
| physics_environment | `CPhysicsEnvironment::SetQuickDelete(bool)` | identical |  |
| physics_environment | `CPhysicsEnvironment::GetDebugOverlay()` | identical |  |
| physics_environment | `CPhysicsEnvironment::SweepCollideable(CPhysCollide const*, Vector const&, Vector const&, QAngle const&, unsigned int, IPhysicsTraceFilter*, CGameTrace` | identical |  |
| physics_environment | `CPhysicsEnvironment::CleanupDeleteList()` | identical |  |
| physics_environment | `CPhysicsEnvironment::EnableDeleteQueue(bool)` | identical |  |
| physics_environment | `CPhysicsEnvironment::SetCollisionSolver(IPhysicsCollisionSolver*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::CreateConstraintGroup(constraint_groupparams_t const&)` | identical |  |
| physics_environment | `CPhysicsEnvironment::CreateFixedConstraint(IPhysicsObject*, IPhysicsObject*, IPhysicsConstraintGroup*, constraint_fixedparams_t const&)` | identical |  |
| physics_environment | `CPhysicsEnvironment::CreateHingeConstraint(IPhysicsObject*, IPhysicsObject*, IPhysicsConstraintGroup*, constraint_hingeparams_t const&)` | identical |  |
| physics_environment | `CPhysicsEnvironment::SetObjectEventHandler(IPhysicsObjectEvent*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::CreateLengthConstraint(IPhysicsObject*, IPhysicsObject*, IPhysicsConstraintGroup*, constraint_lengthparams_t const&)` | identical |  |
| physics_environment | `CPhysicsEnvironment::CreateMotionController(IMotionEvent*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::CreatePulleyConstraint(IPhysicsObject*, IPhysicsObject*, IPhysicsConstraintGroup*, constraint_pulleyparams_t const&)` | identical |  |
| physics_environment | `CPhysicsEnvironment::DestroyConstraintGroup(IPhysicsConstraintGroup*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::EnableConstraintNotify(bool)` | identical |  |
| physics_environment | `CPhysicsEnvironment::CreateRagdollConstraint(IPhysicsObject*, IPhysicsObject*, IPhysicsConstraintGroup*, constraint_ragdollparams_t const&)` | identical |  |
| physics_environment | `CPhysicsEnvironment::CreateSlidingConstraint(IPhysicsObject*, IPhysicsObject*, IPhysicsConstraintGroup*, constraint_slidingparams_t const&)` | identical |  |
| physics_environment | `CPhysicsEnvironment::DestroyMotionController(IPhysicsMotionController*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::DestroyShadowController(IPhysicsShadowController*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::SerializeObjectToBuffer(IPhysicsObject*, unsigned char*, unsigned int)` | identical |  |
| physics_environment | `CPhysicsEnvironment::DestroyVehicleController(IPhysicsVehicleController*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::GetCollisionEventHandler()` | identical |  |
| physics_environment | `CPhysicsEnvironment::NotifyConstraintDisabled(IPhysicsConstraint*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::SetCollisionEventHandler(IPhysicsCollisionEvent*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::SetConstraintEventHandler(IPhysicsConstraintEvent*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::CreateBallsocketConstraint(IPhysicsObject*, IPhysicsObject*, IPhysicsConstraintGroup*, constraint_ballsocketparams_t const&)` | identical |  |
| physics_environment | `CPhysicsEnvironment::CreateLimitedHingeConstraint(IPhysicsObject*, IPhysicsObject*, IPhysicsConstraintGroup*, constraint_limitedhingeparams_t const&)` | identical |  |
| physics_environment | `CPhysicsEnvironment::TraceBox(CGameTrace*, Vector const&, Vector const&, Vector const&, Vector const&)` | identical |  |
| physics_environment | `CPhysicsEnvironment::TraceRay(Ray_t const&, unsigned int, IPhysicsTraceFilter*, CGameTrace*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::~CPhysicsEnvironment()` | identical |  |
| physics_environment | `CVPhysicsDebugOverlay::AddBoxOverlay(Vector const&, Vector const&, Vector const&, QAngle const&, int, int, int, int, float)` | identical |  |
| physics_environment | `CVPhysicsDebugOverlay::AddLineOverlay(Vector const&, Vector const&, int, int, int, bool, float)` | identical |  |
| physics_environment | `CVPhysicsDebugOverlay::AddTextOverlay(Vector const&, float, char const*, ...)` | identical |  |
| physics_environment | `CVPhysicsDebugOverlay::AddTextOverlay(Vector const&, int, float, char const*, ...)` | identical |  |
| physics_environment | `CVPhysicsDebugOverlay::AddTextOverlayRGB(Vector const&, int, float, float, float, float, float, char const*, ...)` | identical |  |
| physics_environment | `CVPhysicsDebugOverlay::AddSweptBoxOverlay(Vector const&, Vector const&, Vector const&, Vector const&, QAngle const&, int, int, int, int, float)` | identical |  |
| physics_environment | `CVPhysicsDebugOverlay::AddTriangleOverlay(Vector const&, Vector const&, Vector const&, int, int, int, int, bool, float)` | identical |  |
| physics_environment | `CVPhysicsDebugOverlay::AddEntityTextOverlay(int, int, float, int, int, int, int, char const*, ...)` | identical |  |
| physics_environment | `CVPhysicsDebugOverlay::AddScreenTextOverlay(float, float, float, int, int, int, int, char const*)` | identical |  |
| physics_environment | `CEmptyCollisionListener::StartTouch(IPhysicsObject*, IPhysicsObject*, IPhysicsCollisionData*)` | identical |  |
| physics_environment | `CEmptyCollisionListener::PreCollision(vcollisionevent_t*)` | identical |  |
| physics_environment | `CEmptyCollisionListener::FluidEndTouch(IPhysicsObject*, IPhysicsFluidController*)` | identical |  |
| physics_environment | `CEmptyCollisionListener::PostCollision(vcollisionevent_t*)` | identical |  |
| physics_environment | `CEmptyCollisionListener::FluidStartTouch(IPhysicsObject*, IPhysicsFluidController*)` | identical |  |
| physics_environment | `CEmptyCollisionListener::ObjectEnterTrigger(IPhysicsObject*, IPhysicsObject*)` | identical |  |
| physics_environment | `CEmptyCollisionListener::ObjectLeaveTrigger(IPhysicsObject*, IPhysicsObject*)` | identical |  |
| physics_environment | `CEmptyCollisionListener::PostSimulationFrame()` | identical |  |
| physics_environment | `CEmptyCollisionListener::EndTouch(IPhysicsObject*, IPhysicsObject*, IPhysicsCollisionData*)` | identical |  |
| physics_environment | `CEmptyCollisionListener::Friction(IPhysicsObject*, float, int, int, IPhysicsCollisionData*)` | identical |  |
| physics_environment | `CPhysicsListenerCollision::CorePairLessFunc(CPhysicsListenerCollision::corepair_t const&, CPhysicsListenerCollision::corepair_t const&)` | identical |  |
| physics_environment | `CPhysicsListenerCollision::core_left_volume(IVP_Controller_Phantom*, IVP_Core*)` | identical |  |
| physics_environment | `CPhysicsListenerCollision::core_entered_volume(IVP_Controller_Phantom*, IVP_Core*)` | identical |  |
| physics_environment | `CPhysicsListenerCollision::mindist_left_volume(IVP_Controller_Phantom*, IVP_Mindist_Base*)` | identical |  |
| physics_environment | `CPhysicsListenerCollision::mindist_entered_volume(IVP_Controller_Phantom*, IVP_Mindist_Base*)` | identical |  |
| physics_environment | `CPhysicsListenerCollision::event_collision_object_deleted(IVP_Real_Object*)` | identical |  |
| physics_environment | `CPhysicsListenerCollision::phantom_is_going_to_be_deleted_event(IVP_Controller_Phantom*)` | identical |  |
| physics_environment | `CPhysicsListenerConstraint::event_constraint_broken(IVP_Constraint*)` | identical |  |
| physics_environment | `CPhysicsListenerConstraint::event_constraint_broken(hk_Breakable_Constraint*)` | identical |  |
| physics_environment | `CPhysicsEnvironment::GetAirDensity() const` | identical |  |
| physics_environment | `CPhysicsEnvironment::GetObjectList(int*) const` | identical |  |
| physics_environment | `CPhysicsEnvironment::IsInSimulation() const` | identical |  |
| physics_environment | `CPhysicsEnvironment::GetActiveObjects(IPhysicsObject**) const` | identical |  |
| physics_environment | `CPhysicsEnvironment::GetNextFrameTime() const` | identical |  |
| physics_environment | `CPhysicsEnvironment::GetSimulationTime() const` | identical |  |
| physics_environment | `CPhysicsEnvironment::ShouldQuickDelete() const` | identical |  |
| physics_environment | `CPhysicsEnvironment::GetActiveObjectCount() const` | identical |  |
| physics_environment | `CPhysicsEnvironment::IsCollisionModelUsed(CPhysCollide*) const` | identical |  |
| physics_environment | `CPhysicsEnvironment::GetSimulationTimestep() const` | identical |  |
| physics_environment | `CPhysicsEnvironment::GetObjectSerializeSize(IPhysicsObject*) const` | identical |  |
| physics_environment | `non-virtual thunk to CCollisionSolver::inter_penetration(IVP_Mindist*, IVP_Real_Object*, IVP_Real_Object*, double)` | identical |  |
| physics_environment | `non-virtual thunk to CCollisionSolver::environment_will_be_deleted(IVP_Environment*)` | identical |  |
| physics_environment | `non-virtual thunk to CCollisionSolver::max_collision_checks_exceeded(int)` | identical |  |
| physics_environment | `non-virtual thunk to CCollisionSolver::max_contacts_exceeded_check_freezing(IVP_Core**, int)` | identical |  |
| physics_environment | `non-virtual thunk to CCollisionSolver::max_collisions_exceeded_check_freezing(IVP_Anomaly_Limits*, IVP_Core*)` | identical |  |
| physics_environment | `non-virtual thunk to CPhysicsListenerCollision::core_left_volume(IVP_Controller_Phantom*, IVP_Core*)` | identical |  |
| physics_environment | `non-virtual thunk to CPhysicsListenerCollision::core_entered_volume(IVP_Controller_Phantom*, IVP_Core*)` | identical |  |
| physics_environment | `non-virtual thunk to CPhysicsListenerCollision::mindist_left_volume(IVP_Controller_Phantom*, IVP_Mindist_Base*)` | identical |  |
| physics_environment | `non-virtual thunk to CPhysicsListenerCollision::mindist_entered_volume(IVP_Controller_Phantom*, IVP_Mindist_Base*)` | identical |  |
| physics_environment | `non-virtual thunk to CPhysicsListenerCollision::phantom_is_going_to_be_deleted_event(IVP_Controller_Phantom*)` | identical |  |
| physics_fluid | `CreateFluidController(IVP_Environment*, CPhysicsObject*, fluidparams_t*)` | edited |  |
| physics_fluid | `CBuoyancyAttacher::~CBuoyancyAttacher()` | identical |  |
| physics_fluid | `CBuoyancyAttacher::~CBuoyancyAttacher()` | identical |  |
| physics_fluid | `IVP_Attacher_To_Cores<IVP_Controller_Buoyancy>::element_added(IVP_U_Set_Active<IVP_Core>*, IVP_Core*)` | identical |  |
| physics_fluid | `IVP_Attacher_To_Cores<IVP_Controller_Buoyancy>::element_removed(IVP_U_Set_Active<IVP_Core>*, IVP_Core*)` | identical |  |
| physics_fluid | `IVP_Attacher_To_Cores<IVP_Controller_Buoyancy>::~IVP_Attacher_To_Cores()` | identical |  |
| physics_fluid | `IVP_Attacher_To_Cores<IVP_Controller_Buoyancy>::~IVP_Attacher_To_Cores()` | identical |  |
| physics_fluid | `CPhysicsFluidController::WakeAllSleepingObjects()` | identical |  |
| physics_fluid | `CPhysicsFluidController::~CPhysicsFluidController()` | edited |  |
| physics_fluid | `CPhysicsFluidController::~CPhysicsFluidController()` | edited |  |
| physics_fluid | `CLiquidSurfaceDescriptor::calc_liquid_surface(IVP_Environment*, IVP_Core*, IVP_U_Float_Hesse*, IVP_U_Float_Point*)` | identical |  |
| physics_fluid | `CLiquidSurfaceDescriptor::~CLiquidSurfaceDescriptor()` | new |  |
| physics_fluid | `CPhysicsFluidController::GetSurfacePlane(Vector*, float*) const` | edited |  |
| physics_fluid | `SavePhysicsFluidController(physsaveparams_t const&, CPhysicsFluidController*)` | identical |  |
| physics_fluid | `RestorePhysicsFluidController(physrestoreparams_t const&, CPhysicsFluidController**)` | identical |  |
| physics_fluid | `CBuoyancyAttacher::get_parameters_per_core(IVP_Core*)` | identical |  |
| physics_fluid | `IVP_Attacher_To_Cores<IVP_Controller_Buoyancy>::pset_is_going_to_be_deleted(IVP_U_Set_Active<IVP_Core>*)` | identical |  |
| physics_fluid | `CPhysicsFluidController::SetGameData(void*)` | identical |  |
| physics_fluid | `CLiquidSurfaceDescriptor::~CLiquidSurfaceDescriptor()` | new |  |
| physics_fluid | `IVP_Attacher_To_Cores_Buoyancy::get_buoyancy_surface(IVP_Real_Object*)` | identical |  |
| physics_fluid | `CPhysicsFluidController::GetDensity() const` | identical |  |
| physics_fluid | `CPhysicsFluidController::GetContents() const` | identical |  |
| physics_fluid | `CPhysicsFluidController::GetGameData() const` | identical |  |
| physics_friction | `CreateFrictionSnapshot(IVP_Real_Object*)` | identical |  |
| physics_friction | `CUtlMemory<IVP_Real_Object*, int>::Grow(int)` | identical |  |
| physics_friction | `CFrictionSnapshot::GetMaterial(int)` | identical |  |
| physics_friction | `CFrictionSnapshot::GetNormalForce()` | identical |  |
| physics_friction | `CFrictionSnapshot::GetContactPoint(Vector&)` | identical |  |
| physics_friction | `CFrictionSnapshot::GetSurfaceNormal(Vector&)` | identical |  |
| physics_friction | `CFrictionSnapshot::GetEnergyAbsorbed()` | identical |  |
| physics_friction | `CFrictionSnapshot::RecomputeFriction()` | identical |  |
| physics_friction | `CFrictionSnapshot::ClearFrictionForce()` | identical |  |
| physics_friction | `CFrictionSnapshot::MarkContactForDelete()` | identical |  |
| physics_friction | `CFrictionSnapshot::DeleteAllMarkedContacts(bool)` | identical |  |
| physics_friction | `CFrictionSnapshot::~CFrictionSnapshot()` | identical |  |
| physics_friction | `CFrictionSnapshot::~CFrictionSnapshot()` | identical |  |
| physics_friction | `DestroyFrictionSnapshot(IPhysicsFrictionSnapshot*)` | identical |  |
| physics_friction | `CFrictionSnapshot::NextFrictionData()` | identical |  |
| physics_friction | `CFrictionSnapshot::GetFrictionCoefficient()` | identical |  |
| physics_friction | `CFrictionSnapshot::IsValid()` | identical |  |
| physics_friction | `CFrictionSnapshot::GetObject(int)` | identical |  |
| physics_material | `_GLOBAL__I_a` | identical |  |
| physics_material | `__CreateCPhysicsSurfacePropsIPhysicsSurfaceProps_interface()` | identical |  |
| physics_material | `CUtlMemory<CUtlSymbol, int>::Grow(int)` | identical |  |
| physics_material | `CUtlMemory<CSurface, int>::Grow(int)` | identical |  |
| physics_material | `CUtlSymbolTableMT::RemoveAll()` | identical |  |
| physics_material | `CIVPMaterialManager::get_elasticity(IVP_Contact_Situation*)` | identical |  |
| physics_material | `CIVPMaterialManager::get_friction_factor(IVP_Contact_Situation*)` | identical |  |
| physics_material | `CIVPMaterialManager::get_material_by_index(IVP_Real_Object*, IVP_U_Point const*, int)` | identical |  |
| physics_material | `CIVPMaterialManager::~CIVPMaterialManager()` | identical |  |
| physics_material | `CIVPMaterialManager::~CIVPMaterialManager()` | identical |  |
| physics_material | `CPhysicsSurfaceProps::ParseSurfaceData(char const*, char const*)` | identical |  |
| physics_material | `CPhysicsSurfaceProps::AddFileToDatabase(char const*)` | identical |  |
| physics_material | `CPhysicsSurfaceProps::PurgeAllSurfaceData()` | identical |  |
| physics_material | `CPhysicsSurfaceProps::CPhysicsSurfaceProps()` | identical |  |
| physics_material | `CPhysicsSurfaceProps::~CPhysicsSurfaceProps()` | identical |  |
| physics_material | `CPhysicsSurfaceProps::~CPhysicsSurfaceProps()` | identical |  |
| physics_material | `CSurface::get_name()` | identical |  |
| physics_material | `CSurface::~CSurface()` | identical |  |
| physics_material | `CSurface::~CSurface()` | identical |  |
| physics_material | `CUtlSymbolTableMT::Find(char const*) const` | identical |  |
| physics_material | `CUtlSymbolTableMT::String(CUtlSymbol) const` | identical |  |
| physics_material | `CPhysicsSurfaceProps::GetSurfaceIndex(char const*) const` | identical |  |
| physics_material | `CPhysicsSurfaceProps::GetPhysicsProperties(int, float*, float*, float*, float*) const` | identical |  |
| physics_material | `CPhysicsSurfaceProps::GetReservedMaterialName(int) const` | identical |  |
| physics_material | `CPhysicsSurfaceProps::GetIVPManager()` | identical |  |
| physics_material | `CPhysicsSurfaceProps::GetIVPMaterial(int)` | identical |  |
| physics_material | `CPhysicsSurfaceProps::GetSurfaceData(int)` | identical |  |
| physics_material | `CPhysicsSurfaceProps::SetWorldMaterialIndexTable(int*, int)` | identical |  |
| physics_material | `CPhysicsSurfaceProps::~CPhysicsSurfaceProps()` | identical |  |
| physics_material | `IVP_Material_Manager::environment_will_be_deleted(IVP_Environment*)` | identical |  |
| physics_material | `CSurface::get_damping()` | identical |  |
| physics_material | `CSurface::get_adhesion()` | identical |  |
| physics_material | `CSurface::get_elasticity()` | identical |  |
| physics_material | `CSurface::get_friction_factor()` | identical |  |
| physics_material | `CSurface::get_second_friction_factor()` | identical |  |
| physics_material | `CPhysicsSurfaceProps::GetPropName(int) const` | identical |  |
| physics_material | `CPhysicsSurfaceProps::SurfacePropCount() const` | identical |  |
| physics_material | `CPhysicsSurfaceProps::GetIVPMaterialIndex(IVP_Material const*) const` | identical |  |
| physics_material | `CPhysicsSurfaceProps::GetPhysicsParameters(int, surfacephysicsparams_t*) const` | identical |  |
| physics_material | `CPhysicsSurfaceProps::RemapIVPMaterialIndex(int) const` | identical |  |
| physics_material | `CPhysicsSurfaceProps::GetString(unsigned short) const` | identical |  |
| physics_motioncontroller | `_GLOBAL__I_a` | identical |  |
| physics_motioncontroller | `CreateMotionController(CPhysicsEnvironment*, IMotionEvent*)` | identical |  |
| physics_motioncontroller | `SavePhysicsMotionController(physsaveparams_t const&, IPhysicsMotionController*)` | identical |  |
| physics_motioncontroller | `RestorePhysicsMotionController(physrestoreparams_t const&, IPhysicsMotionController**)` | identical |  |
| physics_motioncontroller | `CUtlMemory<IVP_Core*, int>::Grow(int)` | identical |  |
| physics_motioncontroller | `CPhysicsMotionController::WakeObjects()` | identical |  |
| physics_motioncontroller | `CPhysicsMotionController::AttachObject(IPhysicsObject*, bool)` | identical |  |
| physics_motioncontroller | `CPhysicsMotionController::ClearObjects()` | identical |  |
| physics_motioncontroller | `CPhysicsMotionController::DetachObject(IPhysicsObject*)` | identical |  |
| physics_motioncontroller | `CPhysicsMotionController::get_controller_name()` | identical |  |
| physics_motioncontroller | `CPhysicsMotionController::do_simulation_controller(IVP_Event_Sim*, IVP_U_Vector<IVP_Core>*)` | identical |  |
| physics_motioncontroller | `CPhysicsMotionController::core_is_going_to_be_deleted_event(IVP_Core*)` | identical |  |
| physics_motioncontroller | `CPhysicsMotionController::~CPhysicsMotionController()` | identical |  |
| physics_motioncontroller | `CPhysicsMotionController::~CPhysicsMotionController()` | identical |  |
| physics_motioncontroller | `non-virtual thunk to CPhysicsMotionController::WakeObjects()` | identical |  |
| physics_motioncontroller | `non-virtual thunk to CPhysicsMotionController::ClearObjects()` | identical |  |
| physics_motioncontroller | `non-virtual thunk to CPhysicsMotionController::DetachObject(IPhysicsObject*)` | identical |  |
| physics_motioncontroller | `non-virtual thunk to CPhysicsMotionController::~CPhysicsMotionController()` | identical |  |
| physics_motioncontroller | `CPhysicsMotionController::GetObjects(IPhysicsObject**)` | identical |  |
| physics_motioncontroller | `CPhysicsMotionController::SetPriority(IPhysicsMotionController::priority_t)` | identical |  |
| physics_motioncontroller | `CPhysicsMotionController::CountObjects()` | identical |  |
| physics_motioncontroller | `CPhysicsMotionController::SetEventHandler(IMotionEvent*)` | identical |  |
| physics_motioncontroller | `CPhysicsMotionController::get_controller_priority()` | identical |  |
| physics_motioncontroller | `CPhysicsMotionController::~CPhysicsMotionController()` | identical |  |
| physics_motioncontroller | `non-virtual thunk to CPhysicsMotionController::GetObjects(IPhysicsObject**)` | identical |  |
| physics_motioncontroller | `non-virtual thunk to CPhysicsMotionController::SetPriority(IPhysicsMotionController::priority_t)` | identical |  |
| physics_motioncontroller | `non-virtual thunk to CPhysicsMotionController::AttachObject(IPhysicsObject*, bool)` | identical |  |
| physics_motioncontroller | `non-virtual thunk to CPhysicsMotionController::CountObjects()` | identical |  |
| physics_motioncontroller | `non-virtual thunk to CPhysicsMotionController::SetEventHandler(IMotionEvent*)` | identical |  |
| physics_motioncontroller | `non-virtual thunk to CPhysicsMotionController::~CPhysicsMotionController()` | identical |  |
| physics_object | `RestorePhysicsObject(physrestoreparams_t const&, CPhysicsObject**)` | differs | size retail 891 vs ours 695 |
| physics_object | `_GLOBAL__I_a` | identical |  |
| physics_object | `SavePhysicsObject(physsaveparams_t const&, CPhysicsObject*)` | identical |  |
| physics_object | `CreatePhysicsObject(CPhysicsEnvironment*, CPhysCollide const*, int, Vector const&, QAngle const&, objectparams_t*, bool)` | identical |  |
| physics_object | `CreatePhysicsSphere(CPhysicsEnvironment*, float, int, Vector const&, QAngle const&, objectparams_t*, bool)` | identical |  |
| physics_object | `MaterialIndexDataOps()` | identical |  |
| physics_object | `CreateObjectFromBuffer(CPhysicsEnvironment*, void*, unsigned char*, unsigned int, bool)` | identical |  |
| physics_object | `PostRestorePhysicsObject()` | identical |  |
| physics_object | `CreateObjectFromBuffer_UseExistingMemory(CPhysicsEnvironment*, void*, unsigned char*, unsigned int, CPhysicsObject*)` | identical |  |
| physics_object | `InitObjectTemplate(IVP_Template_Real_Object&, int, objectparams_t*, bool)` | identical |  |
| physics_object | `CUtlMemory<postrestore_objectlist_t, int>::Grow(int)` | identical |  |
| physics_object | `CUtlVector<postrestore_objectlist_t, CUtlMemory<postrestore_objectlist_t, int>>::~CUtlVector()` | identical |  |
| physics_object | `CPhysicsObject::EnableDrag(bool)` | identical |  |
| physics_object | `CPhysicsObject::SetInertia(Vector const&)` | identical |  |
| physics_object | `CPhysicsObject::AddVelocity(Vector const*, Vector const*)` | identical |  |
| physics_object | `CPhysicsObject::SetPosition(Vector const&, QAngle const&, bool)` | identical |  |
| physics_object | `CPhysicsObject::SetVelocity(Vector const*, Vector const*)` | identical |  |
| physics_object | `CPhysicsObject::BecomeHinged(int)` | identical |  |
| physics_object | `CPhysicsObject::EnableMotion(bool)` | identical |  |
| physics_object | `CPhysicsObject::RemoveHinged()` | identical |  |
| physics_object | `CPhysicsObject::BecomeTrigger()` | identical |  |
| physics_object | `CPhysicsObject::EnableGravity(bool)` | identical |  |
| physics_object | `CPhysicsObject::WriteToTemplate(vphysics_save_cphysicsobject_t&)` | identical |  |
| physics_object | `CPhysicsObject::ApplyForceCenter(Vector const&)` | identical |  |
| physics_object | `CPhysicsObject::ApplyForceOffset(Vector const&, Vector const&)` | identical |  |
| physics_object | `CPhysicsObject::EnableCollisions(bool)` | identical |  |
| physics_object | `CPhysicsObject::InitFromTemplate(CPhysicsEnvironment*, void*, vphysics_save_cphysicsobject_t const&)` | identical |  |
| physics_object | `CPhysicsObject::SetMaterialIndex(int)` | identical |  |
| physics_object | `CPhysicsObject::ApplyTorqueCenter(Vector const&)` | identical |  |
| physics_object | `CPhysicsObject::SetPositionMatrix(matrix3x4_t const&, bool)` | identical |  |
| physics_object | `CPhysicsObject::RecomputeDragBases()` | identical |  |
| physics_object | `CPhysicsObject::ComputeShadowControl(hlshadowcontrol_params_t const&, float, float)` | identical |  |
| physics_object | `CPhysicsObject::RecheckCollisionFilter()` | identical |  |
| physics_object | `CPhysicsObject::SetVelocityInstantaneous(Vector const*, Vector const*)` | identical |  |
| physics_object | `CPhysicsObject::Init(CPhysCollide const*, IVP_Real_Object*, int, float, float, float)` | identical |  |
| physics_object | `CPhysicsObject::Wake()` | identical |  |
| physics_object | `CPhysicsObject::Sleep()` | identical |  |
| physics_object | `CPhysicsObject::SetMass(float)` | identical |  |
| physics_object | `CPhysicsObject::~CPhysicsObject()` | identical |  |
| physics_object | `CPhysicsObject::~CPhysicsObject()` | identical |  |
| physics_object | `CMaterialIndexOps::Save(SaveRestoreFieldInfo_t const&, ISave*)` | identical |  |
| physics_object | `CMaterialIndexOps::Restore(SaveRestoreFieldInfo_t const&, IRestore*)` | identical |  |
| physics_object | `CPhysicsObject::GetInertia() const` | identical |  |
| physics_object | `CPhysicsObject::GetPosition(Vector*, QAngle*) const` | identical |  |
| physics_object | `CPhysicsObject::GetVelocity(Vector*, Vector*) const` | identical |  |
| physics_object | `CPhysicsObject::LocalToWorld(Vector*, Vector const&) const` | identical |  |
| physics_object | `CPhysicsObject::WorldToLocal(Vector*, Vector const&) const` | identical |  |
| physics_object | `CPhysicsObject::GetInvInertia() const` | identical |  |
| physics_object | `CPhysicsObject::GetContactPoint(Vector*, IPhysicsObject**) const` | identical |  |
| physics_object | `CPhysicsObject::GetSphereRadius() const` | identical |  |
| physics_object | `CPhysicsObject::OutputDebugInfo() const` | identical |  |
| physics_object | `CPhysicsObject::GetPositionMatrix(matrix3x4_t*) const` | identical |  |
| physics_object | `CPhysicsObject::GetShadowPosition(Vector*, QAngle*) const` | identical |  |
| physics_object | `CPhysicsObject::GetDragInDirection(IVP_U_Float_Point const&) const` | identical |  |
| physics_object | `CPhysicsObject::GetVelocityAtPoint(Vector const&, Vector*) const` | identical |  |
| physics_object | `CPhysicsObject::LocalToWorldVector(Vector*, Vector const&) const` | identical |  |
| physics_object | `CPhysicsObject::WorldToLocalVector(Vector*, Vector const&) const` | identical |  |
| physics_object | `CPhysicsObject::CalculateLinearDrag(Vector const&) const` | identical |  |
| physics_object | `CPhysicsObject::GetImplicitVelocity(Vector*, Vector*) const` | identical |  |
| physics_object | `CPhysicsObject::CalculateAngularDrag(Vector const&) const` | identical |  |
| physics_object | `CPhysicsObject::CalculateForceOffset(Vector const&, Vector const&, Vector*, Vector*) const` | identical |  |
| physics_object | `CPhysicsObject::CalculateVelocityOffset(Vector const&, Vector const&, Vector*, Vector*) const` | identical |  |
| physics_object | `CPhysicsObject::GetMassCenterLocalSpace() const` | identical |  |
| physics_object | `CPhysicsObject::GetAngularDragInDirection(IVP_U_Float_Point const&) const` | identical |  |
| physics_object | `CPhysicsObject::GetEnergy() const` | identical |  |
| physics_object | `CPhysicsObject::NotifyWake()` | identical |  |
| physics_object | `CPhysicsObject::SetDamping(float const*, float const*)` | identical |  |
| physics_object | `CPhysicsObject::NotifySleep()` | identical |  |
| physics_object | `CPhysicsObject::SetContents(unsigned int)` | identical |  |
| physics_object | `CPhysicsObject::SetGameData(void*)` | identical |  |
| physics_object | `CPhysicsObject::SetGameFlags(unsigned short)` | identical |  |
| physics_object | `CPhysicsObject::SetGameIndex(unsigned short)` | identical |  |
| physics_object | `CPhysicsObject::UpdateShadow(Vector const&, QAngle const&, bool, float)` | identical |  |
| physics_object | `CPhysicsObject::RemoveTrigger()` | identical |  |
| physics_object | `CPhysicsObject::SetBuoyancyRatio(float)` | identical |  |
| physics_object | `CPhysicsObject::SetCallbackFlags(unsigned short)` | identical |  |
| physics_object | `CPhysicsObject::SetDragCoefficient(float*, float*)` | identical |  |
| physics_object | `CPhysicsObject::RecheckContactPoints()` | identical |  |
| physics_object | `CPhysicsObject::CreateFrictionSnapshot()` | identical |  |
| physics_object | `CPhysicsObject::GetVPhysicsEnvironment()` | identical |  |
| physics_object | `CPhysicsObject::RemoveShadowController()` | identical |  |
| physics_object | `CPhysicsObject::DestroyFrictionSnapshot(IPhysicsFrictionSnapshot*)` | identical |  |
| physics_object | `CPhysicsObject::SetShadow(float, float, bool, bool)` | identical |  |
| physics_object | `CPhysicsObject::~CPhysicsObject()` | identical |  |
| physics_object | `CMaterialIndexOps::IsEmpty(SaveRestoreFieldInfo_t const&)` | identical |  |
| physics_object | `CMaterialIndexOps::MakeEmpty(SaveRestoreFieldInfo_t const&)` | identical |  |
| physics_object | `CDefSaveRestoreOps::Parse(SaveRestoreFieldInfo_t const&, char const*)` | identical |  |
| physics_object | `CPhysicsObject::GetCollide() const` | identical |  |
| physics_object | `CPhysicsObject::GetDamping(float*, float*) const` | identical |  |
| physics_object | `CPhysicsObject::GetInvMass() const` | identical |  |
| physics_object | `CPhysicsObject::IsMoveable() const` | identical |  |
| physics_object | `CPhysicsObject::GetContents() const` | identical |  |
| physics_object | `CPhysicsObject::GetGameData() const` | identical |  |
| physics_object | `CPhysicsObject::GetGameFlags() const` | identical |  |
| physics_object | `CPhysicsObject::GetGameIndex() const` | identical |  |
| physics_object | `CPhysicsObject::IsDragEnabled() const` | identical |  |
| physics_object | `CPhysicsObject::IsMotionEnabled() const` | identical |  |
| physics_object | `CPhysicsObject::GetCallbackFlags() const` | identical |  |
| physics_object | `CPhysicsObject::GetMaterialIndex() const` | identical |  |
| physics_object | `CPhysicsObject::IsGravityEnabled() const` | identical |  |
| physics_object | `CPhysicsObject::IsCollisionEnabled() const` | identical |  |
| physics_object | `CPhysicsObject::IsControlledByGame() const` | identical |  |
| physics_object | `CPhysicsObject::GetShadowController() const` | identical |  |
| physics_object | `CPhysicsObject::IsAttachedToConstraint(bool) const` | identical |  |
| physics_object | `CPhysicsObject::GetMass() const` | identical |  |
| physics_object | `CPhysicsObject::GetName() const` | identical |  |
| physics_object | `CPhysicsObject::IsFluid() const` | identical |  |
| physics_object | `CPhysicsObject::IsAsleep() const` | identical |  |
| physics_object | `CPhysicsObject::IsHinged() const` | identical |  |
| physics_object | `CPhysicsObject::IsStatic() const` | identical |  |
| physics_object | `CPhysicsObject::IsTrigger() const` | identical |  |
| physics_shadow | `_GLOBAL__I_a` | identical |  |
| physics_shadow | `datamap_t* DataMapInit<vphysics_save_cshadowcontroller_t>(vphysics_save_cshadowcontroller_t*)` | identical |  |
| physics_shadow | `ComputeController(IVP_U_Float_Point&, IVP_U_Float_Point const&, IVP_U_Float_Point const&, float, float, IVP_U_Float_Point*)` | identical |  |
| physics_shadow | `ComputeController(IVP_U_Float_Point&, IVP_U_Float_Point const&, float, float, float, float, IVP_U_Float_Point*)` | identical |  |
| physics_shadow | `CreatePlayerController(CPhysicsObject*)` | identical |  |
| physics_shadow | `CreateShadowController(CPhysicsObject*, bool, bool)` | identical |  |
| physics_shadow | `ComputeShadowControllerHL(CPhysicsObject*, hlshadowcontrol_params_t const&, float, float)` | identical |  |
| physics_shadow | `ComputeShadowControllerIVP(IVP_Real_Object*, shadowcontrol_params_t&, float, float)` | identical |  |
| physics_shadow | `SavePhysicsShadowController(physsaveparams_t const&, IPhysicsShadowController*)` | identical |  |
| physics_shadow | `RestorePhysicsShadowControllerInternal(physrestoreparams_t const&, IPhysicsShadowController**, CPhysicsObject*)` | identical |  |
| physics_shadow | `ControlPhysicsPlayerControllerAttachment_Silent(IPhysicsPlayerController*, IVP_Real_Object*, bool)` | identical |  |
| physics_shadow | `ControlPhysicsShadowControllerAttachment_Silent(IPhysicsShadowController*, IVP_Real_Object*, bool)` | identical |  |
| physics_shadow | `CNormalList::ClampVector(Vector const&, float)` | identical |  |
| physics_shadow | `CPlayerController::IsInContact()` | identical |  |
| physics_shadow | `CPlayerController::GetLastImpulse(Vector*)` | identical |  |
| physics_shadow | `CPlayerController::GetShadowPosition(Vector*, QAngle*)` | identical |  |
| physics_shadow | `CPlayerController::GetShadowVelocity(Vector*)` | identical |  |
| physics_shadow | `CPlayerController::TryTeleportObject()` | identical |  |
| physics_shadow | `CPlayerController::get_controller_name()` | identical |  |
| physics_shadow | `CPlayerController::do_simulation_controller(IVP_Event_Sim*, IVP_U_Vector<IVP_Core>*)` | identical |  |
| physics_shadow | `CPlayerController::StepUp(float)` | identical |  |
| physics_shadow | `CPlayerController::Update(Vector const&, Vector const&, float, bool, IPhysicsObject*)` | identical |  |
| physics_shadow | `CPlayerController::MaxSpeed(Vector const&)` | identical |  |
| physics_shadow | `CPlayerController::SetObject(IPhysicsObject*)` | identical |  |
| physics_shadow | `CPlayerController::CPlayerController(CPhysicsObject*)` | identical |  |
| physics_shadow | `CPlayerController::~CPlayerController()` | identical |  |
| physics_shadow | `CPlayerController::~CPlayerController()` | identical |  |
| physics_shadow | `CShadowController::AttachObject()` | identical |  |
| physics_shadow | `CShadowController::DetachObject()` | identical |  |
| physics_shadow | `CShadowController::GetLastImpulse(Vector*)` | identical |  |
| physics_shadow | `CShadowController::InitFromTemplate(vphysics_save_cshadowcontroller_t const&)` | identical |  |
| physics_shadow | `CShadowController::GetTargetPosition(Vector*, QAngle*)` | identical |  |
| physics_shadow | `CShadowController::GetTeleportDistance()` | identical |  |
| physics_shadow | `CShadowController::SetTeleportDistance(float)` | identical |  |
| physics_shadow | `CShadowController::get_controller_name()` | identical |  |
| physics_shadow | `CShadowController::do_simulation_controller(IVP_Event_Sim*, IVP_U_Vector<IVP_Core>*)` | identical |  |
| physics_shadow | `CShadowController::StepUp(float)` | identical |  |
| physics_shadow | `CShadowController::Update(Vector const&, QAngle const&, float)` | identical |  |
| physics_shadow | `CShadowController::~CShadowController()` | identical |  |
| physics_shadow | `CShadowController::~CShadowController()` | identical |  |
| physics_shadow | `non-virtual thunk to CPlayerController::IsInContact()` | identical |  |
| physics_shadow | `non-virtual thunk to CPlayerController::GetLastImpulse(Vector*)` | identical |  |
| physics_shadow | `non-virtual thunk to CPlayerController::GetShadowPosition(Vector*, QAngle*)` | identical |  |
| physics_shadow | `non-virtual thunk to CPlayerController::GetShadowVelocity(Vector*)` | identical |  |
| physics_shadow | `non-virtual thunk to CPlayerController::StepUp(float)` | identical |  |
| physics_shadow | `non-virtual thunk to CPlayerController::~CPlayerController()` | identical |  |
| physics_shadow | `non-virtual thunk to CShadowController::GetLastImpulse(Vector*)` | identical |  |
| physics_shadow | `non-virtual thunk to CShadowController::GetTargetPosition(Vector*, QAngle*)` | identical |  |
| physics_shadow | `non-virtual thunk to CShadowController::GetTeleportDistance()` | identical |  |
| physics_shadow | `non-virtual thunk to CShadowController::SetTeleportDistance(float)` | identical |  |
| physics_shadow | `non-virtual thunk to CShadowController::StepUp(float)` | identical |  |
| physics_shadow | `non-virtual thunk to CShadowController::~CShadowController()` | identical |  |
| physics_shadow | `non-virtual thunk to CShadowController::~CShadowController()` | identical |  |
| physics_shadow | `non-virtual thunk to CPlayerController::~CPlayerController()` | identical |  |
| physics_shadow | `DestroyPlayerController(IPhysicsPlayerController*)` | identical |  |
| physics_shadow | `SavePhysicsPlayerController(physsaveparams_t const&, CPlayerController*)` | identical |  |
| physics_shadow | `RestorePhysicsPlayerController(physrestoreparams_t const&, CPlayerController**)` | identical |  |
| physics_shadow | `RestorePhysicsShadowController(physrestoreparams_t const&, IPhysicsShadowController**)` | identical |  |
| physics_shadow | `CPlayerController::SetEventHandler(IPhysicsPlayerControllerEvent*)` | identical |  |
| physics_shadow | `CPlayerController::GetPushMassLimit()` | identical |  |
| physics_shadow | `CPlayerController::SetPushMassLimit(float)` | identical |  |
| physics_shadow | `CPlayerController::GetPushSpeedLimit()` | identical |  |
| physics_shadow | `CPlayerController::SetPushSpeedLimit(float)` | identical |  |
| physics_shadow | `CPlayerController::event_object_frozen(IVP_Event_Object*)` | identical |  |
| physics_shadow | `CPlayerController::event_object_created(IVP_Event_Object*)` | identical |  |
| physics_shadow | `CPlayerController::event_object_deleted(IVP_Event_Object*)` | identical |  |
| physics_shadow | `CPlayerController::event_object_revived(IVP_Event_Object*)` | identical |  |
| physics_shadow | `CPlayerController::get_controller_priority()` | identical |  |
| physics_shadow | `CPlayerController::Jump()` | identical |  |
| physics_shadow | `CPlayerController::GetObject()` | identical |  |
| physics_shadow | `CPlayerController::WasFrozen()` | identical |  |
| physics_shadow | `CPlayerController::~CPlayerController()` | identical |  |
| physics_shadow | `CShadowController::GetMaxSpeed(float*, float*)` | identical |  |
| physics_shadow | `CShadowController::AllowsRotation()` | identical |  |
| physics_shadow | `CShadowController::AllowsTranslation()` | identical |  |
| physics_shadow | `CShadowController::UseShadowMaterial(bool)` | identical |  |
| physics_shadow | `CShadowController::ObjectMaterialChanged(int)` | identical |  |
| physics_shadow | `CShadowController::IsPhysicallyControlled()` | identical |  |
| physics_shadow | `CShadowController::SetPhysicallyControlled(bool)` | identical |  |
| physics_shadow | `CShadowController::get_controller_priority()` | identical |  |
| physics_shadow | `CShadowController::MaxSpeed(float, float)` | identical |  |
| physics_shadow | `non-virtual thunk to CPlayerController::SetEventHandler(IPhysicsPlayerControllerEvent*)` | identical |  |
| physics_shadow | `non-virtual thunk to CPlayerController::GetPushMassLimit()` | identical |  |
| physics_shadow | `non-virtual thunk to CPlayerController::SetPushMassLimit(float)` | identical |  |
| physics_shadow | `non-virtual thunk to CPlayerController::GetPushSpeedLimit()` | identical |  |
| physics_shadow | `non-virtual thunk to CPlayerController::SetPushSpeedLimit(float)` | identical |  |
| physics_shadow | `non-virtual thunk to CPlayerController::Jump()` | identical |  |
| physics_shadow | `non-virtual thunk to CPlayerController::Update(Vector const&, Vector const&, float, bool, IPhysicsObject*)` | identical |  |
| physics_shadow | `non-virtual thunk to CPlayerController::MaxSpeed(Vector const&)` | identical |  |
| physics_shadow | `non-virtual thunk to CPlayerController::GetObject()` | identical |  |
| physics_shadow | `non-virtual thunk to CPlayerController::SetObject(IPhysicsObject*)` | identical |  |
| physics_shadow | `non-virtual thunk to CPlayerController::WasFrozen()` | identical |  |
| physics_shadow | `non-virtual thunk to CPlayerController::~CPlayerController()` | identical |  |
| physics_shadow | `non-virtual thunk to CShadowController::GetMaxSpeed(float*, float*)` | identical |  |
| physics_shadow | `non-virtual thunk to CShadowController::AllowsRotation()` | identical |  |
| physics_shadow | `non-virtual thunk to CShadowController::AllowsTranslation()` | identical |  |
| physics_shadow | `non-virtual thunk to CShadowController::UseShadowMaterial(bool)` | identical |  |
| physics_shadow | `non-virtual thunk to CShadowController::ObjectMaterialChanged(int)` | identical |  |
| physics_shadow | `non-virtual thunk to CShadowController::IsPhysicallyControlled()` | identical |  |
| physics_shadow | `non-virtual thunk to CShadowController::SetPhysicallyControlled(bool)` | identical |  |
| physics_shadow | `non-virtual thunk to CShadowController::Update(Vector const&, QAngle const&, float)` | identical |  |
| physics_shadow | `non-virtual thunk to CShadowController::MaxSpeed(float, float)` | identical |  |
| physics_shadow | `non-virtual thunk to CPlayerController::event_object_frozen(IVP_Event_Object*)` | identical |  |
| physics_shadow | `non-virtual thunk to CPlayerController::event_object_created(IVP_Event_Object*)` | identical |  |
| physics_shadow | `non-virtual thunk to CPlayerController::event_object_deleted(IVP_Event_Object*)` | identical |  |
| physics_shadow | `non-virtual thunk to CPlayerController::event_object_revived(IVP_Event_Object*)` | identical |  |
| physics_shadow | `non-virtual thunk to CPlayerController::~CPlayerController()` | identical |  |
| physics_spring | `_GLOBAL__I_a` | identical |  |
| physics_spring | `CreateSpring(IVP_Environment*, CPhysicsObject*, CPhysicsObject*, springparams_t*)` | identical |  |
| physics_spring | `SavePhysicsSpring(physsaveparams_t const&, CPhysicsSpring*)` | identical |  |
| physics_spring | `RestorePhysicsSpring(physrestoreparams_t const&, CPhysicsSpring**)` | identical |  |
| physics_spring | `CPhysicsSpring::GetEndpoints(Vector*, Vector*)` | identical |  |
| physics_spring | `CPhysicsSpring::SetSpringLength(float)` | identical |  |
| physics_spring | `CPhysicsSpring::SetSpringDamping(float)` | identical |  |
| physics_spring | `CPhysicsSpring::SetSpringConstant(float)` | identical |  |
| physics_spring | `CPhysicsSpring::event_object_deleted(IVP_Event_Object*)` | identical |  |
| physics_spring | `CPhysicsSpring::~CPhysicsSpring()` | identical |  |
| physics_spring | `CPhysicsSpring::~CPhysicsSpring()` | identical |  |
| physics_spring | `non-virtual thunk to CPhysicsSpring::event_object_deleted(IVP_Event_Object*)` | identical |  |
| physics_spring | `non-virtual thunk to CPhysicsSpring::~CPhysicsSpring()` | identical |  |
| physics_spring | `CPhysicsSpring::GetEndObject()` | identical |  |
| physics_spring | `CPhysicsSpring::GetStartObject()` | identical |  |
| physics_spring | `CPhysicsSpring::event_object_frozen(IVP_Event_Object*)` | identical |  |
| physics_spring | `CPhysicsSpring::event_object_created(IVP_Event_Object*)` | identical |  |
| physics_spring | `CPhysicsSpring::event_object_revived(IVP_Event_Object*)` | identical |  |
| physics_spring | `CPhysicsSpring::~CPhysicsSpring()` | identical |  |
| physics_spring | `non-virtual thunk to CPhysicsSpring::event_object_frozen(IVP_Event_Object*)` | identical |  |
| physics_spring | `non-virtual thunk to CPhysicsSpring::event_object_created(IVP_Event_Object*)` | identical |  |
| physics_spring | `non-virtual thunk to CPhysicsSpring::event_object_revived(IVP_Event_Object*)` | identical |  |
| physics_spring | `non-virtual thunk to CPhysicsSpring::~CPhysicsSpring()` | identical |  |
| physics_vehicle | `_GLOBAL__I_a` | identical |  |
| physics_vehicle | `datamap_t* DataMapInit<vehicle_wheelparams_t>(vehicle_wheelparams_t*)` | identical |  |
| physics_vehicle | `datamap_t* DataMapInit<vehicle_operatingparams_t>(vehicle_operatingparams_t*)` | identical |  |
| physics_vehicle | `CreateVehicleController(CPhysicsEnvironment*, CPhysicsObject*, vehicleparams_t const&, unsigned int, IPhysicsGameTrace*)` | identical |  |
| physics_vehicle | `SavePhysicsVehicleController(physsaveparams_t const&, CVehicleController*)` | identical |  |
| physics_vehicle | `RestorePhysicsVehicleController(physrestoreparams_t const&, CVehicleController**)` | identical |  |
| physics_vehicle | `ShouldOverrideWheelContactFriction(float*, IVP_Real_Object*, IVP_Real_Object*, IVP_U_Float_Point*)` | identical |  |
| physics_vehicle | `CVehicleController::CalcEngine(float, float, bool, float, bool)` | identical |  |
| physics_vehicle | `CVehicleController::CreateWheel(int, vehicle_axleparams_t&)` | identical |  |
| physics_vehicle | `CVehicleController::CalcSteering(float, float, float, bool)` | identical |  |
| physics_vehicle | `CVehicleController::UpdateEngine(vehicle_controlparams_t const&, float, float, float, bool, bool)` | identical |  |
| physics_vehicle | `CVehicleController::InitCarSystem(CPhysicsObject*)` | identical |  |
| physics_vehicle | `CVehicleController::UpdateSkidding(bool)` | identical |  |
| physics_vehicle | `CVehicleController::CreateTraceData(int, vehicle_axleparams_t&)` | identical |  |
| physics_vehicle | `CVehicleController::InitVehicleData(vehicleparams_t const&)` | identical |  |
| physics_vehicle | `CVehicleController::UpdateHandbrake(vehicle_controlparams_t const&, float, bool, bool)` | identical |  |
| physics_vehicle | `CVehicleController::WriteToTemplate(vphysics_save_cvehiclecontroller_t&)` | identical |  |
| physics_vehicle | `CVehicleController::CreateIVPObjects()` | identical |  |
| physics_vehicle | `CVehicleController::InitFromTemplate(CPhysicsEnvironment*, void*, IPhysicsGameTrace*, vphysics_save_cvehiclecontroller_t const&)` | identical |  |
| physics_vehicle | `CVehicleController::SetWheelFriction(int, float)` | identical |  |
| physics_vehicle | `CVehicleController::UpdatePowerslide(vehicle_controlparams_t const&, bool, float)` | identical |  |
| physics_vehicle | `CVehicleController::UpdateExtraForces()` | identical |  |
| physics_vehicle | `CVehicleController::VehicleDataReload()` | identical |  |
| physics_vehicle | `CVehicleController::InitCarSystemWheels(IVP_Template_Car_System&)` | identical |  |
| physics_vehicle | `CVehicleController::GetWheelContactPoint(int, Vector*, int*)` | identical |  |
| physics_vehicle | `CVehicleController::GetCarSystemDebugData(vehicle_debugcarsystem_t&)` | identical |  |
| physics_vehicle | `CVehicleController::CalcEngineTransmission(float)` | identical |  |
| physics_vehicle | `CVehicleController::Update(float, vehicle_controlparams_t&)` | identical |  |
| physics_vehicle | `CVehicleController::GetWheel(int)` | identical |  |
| physics_vehicle | `CVehicleController::~CVehicleController()` | identical |  |
| physics_vehicle | `CVehicleController::~CVehicleController()` | identical |  |
| physics_vehicle | `non-virtual thunk to CVehicleController::~CVehicleController()` | identical |  |
| physics_vehicle | `non-virtual thunk to CVehicleController::~CVehicleController()` | identical |  |
| physics_vehicle | `CVehicleController::IsBoosting()` | identical |  |
| physics_vehicle | `CVehicleController::GetWheelCount()` | identical |  |
| physics_vehicle | `CVehicleController::OnVehicleExit()` | identical |  |
| physics_vehicle | `CVehicleController::UpdateBooster(float)` | identical |  |
| physics_vehicle | `CVehicleController::OnVehicleEnter()` | identical |  |
| physics_vehicle | `CVehicleController::SetSpringLength(int, float)` | identical |  |
| physics_vehicle | `CVehicleController::GetVehicleParams()` | identical |  |
| physics_vehicle | `CVehicleController::IsEngineDisabled()` | identical |  |
| physics_vehicle | `CVehicleController::SetEngineDisabled(bool)` | identical |  |
| physics_vehicle | `CVehicleController::GetOperatingParams()` | identical |  |
| physics_vehicle | `CVehicleController::event_object_frozen(IVP_Event_Object*)` | identical |  |
| physics_vehicle | `CVehicleController::event_object_created(IVP_Event_Object*)` | identical |  |
| physics_vehicle | `CVehicleController::event_object_deleted(IVP_Event_Object*)` | identical |  |
| physics_vehicle | `CVehicleController::event_object_revived(IVP_Event_Object*)` | identical |  |
| physics_vehicle | `CVehicleController::GetVehicleParamsForChange()` | identical |  |
| physics_vehicle | `non-virtual thunk to CVehicleController::event_object_frozen(IVP_Event_Object*)` | identical |  |
| physics_vehicle | `non-virtual thunk to CVehicleController::event_object_created(IVP_Event_Object*)` | identical |  |
| physics_vehicle | `non-virtual thunk to CVehicleController::event_object_deleted(IVP_Event_Object*)` | identical |  |
| physics_vehicle | `non-virtual thunk to CVehicleController::event_object_revived(IVP_Event_Object*)` | identical |  |
| physics_virtualmesh | `CPhysCollideVirtualMesh::CreateBoundingSurfaceFromRange(virtualmeshlist_t const&, int, int)` | differs | size retail 810 vs ours 800 |
| physics_virtualmesh | `_GLOBAL__I_a` | identical |  |
| physics_virtualmesh | `CreateVirtualMesh(virtualmeshparams_t const&)` | identical |  |
| physics_virtualmesh | `FlushFrameLocks()` | identical |  |
| physics_virtualmesh | `CUtlMemory<CPhysCollideVirtualMesh*, int>::Grow(int)` | identical |  |
| physics_virtualmesh | `CDataManager<CMeshInstance, virtualmeshlist_t, CMeshInstance*, CThreadFastMutex>::CreateResource(virtualmeshlist_t const&, bool)` | identical |  |
| physics_virtualmesh | `CDataManager<CMeshInstance, virtualmeshlist_t, CMeshInstance*, CThreadFastMutex>::DestroyResourceStorage(void*)` | identical |  |
| physics_virtualmesh | `CDataManager<CMeshInstance, virtualmeshlist_t, CMeshInstance*, CThreadFastMutex>::Lock()` | identical |  |
| physics_virtualmesh | `CDataManager<CMeshInstance, virtualmeshlist_t, CMeshInstance*, CThreadFastMutex>::Unlock()` | identical |  |
| physics_virtualmesh | `CDataManager<CMeshInstance, virtualmeshlist_t, CMeshInstance*, CThreadFastMutex>::TryLock()` | identical |  |
| physics_virtualmesh | `CDataManager<CMeshInstance, virtualmeshlist_t, CMeshInstance*, CThreadFastMutex>::~CDataManager()` | identical |  |
| physics_virtualmesh | `CDataManager<CMeshInstance, virtualmeshlist_t, CMeshInstance*, CThreadFastMutex>::~CDataManager()` | identical |  |
| physics_virtualmesh | `CMeshInstance::Init(virtualmeshlist_t const&)` | identical |  |
| physics_virtualmesh | `CThreadLocalPtr<CUtlVector<CPhysCollideVirtualMesh*, CUtlMemory<CPhysCollideVirtualMesh*, int>>>::~CThreadLocalPtr()` | identical |  |
| physics_virtualmesh | `CPhysCollideVirtualMesh::FrameRelease()` | identical |  |
| physics_virtualmesh | `CPhysCollideVirtualMesh::CreateMeshBoundingHull(virtualmeshlist_t const&)` | identical |  |
| physics_virtualmesh | `CPhysCollideVirtualMesh::GetAllLedgesWithinRadius(IVP_U_Point const*, double, IVP_U_BigVector<IVP_Compact_Ledge>*, IVP_Compact_Ledge const*)` | identical |  |
| physics_virtualmesh | `CPhysCollideVirtualMesh::AddRef()` | identical |  |
| physics_virtualmesh | `CPhysCollideVirtualMesh::~CPhysCollideVirtualMesh()` | identical |  |
| physics_virtualmesh | `CPhysCollideVirtualMesh::~CPhysCollideVirtualMesh()` | identical |  |
| physics_virtualmesh | `IVP_SurfaceManager_VirtualMesh::add_reference_to_ledge(IVP_Compact_Ledge const*)` | identical |  |
| physics_virtualmesh | `IVP_SurfaceManager_VirtualMesh::remove_reference_to_ledge(IVP_Compact_Ledge const*)` | identical |  |
| physics_virtualmesh | `IVP_SurfaceManager_VirtualMesh::get_all_ledges_within_radius(IVP_U_Point const*, double, IVP_Compact_Ledge const*, IVP_Real_Object*, IVP_Compact_Ledge` | identical |  |
| physics_virtualmesh | `IVP_SurfaceManager_VirtualMesh::insert_all_ledges_hitting_ray(IVP_Ray_Solver*, IVP_Real_Object*)` | identical |  |
| physics_virtualmesh | `IVP_SurfaceManager_VirtualMesh::~IVP_SurfaceManager_VirtualMesh()` | identical |  |
| physics_virtualmesh | `IVP_SurfaceManager_VirtualMesh::~IVP_SurfaceManager_VirtualMesh()` | identical |  |
| physics_virtualmesh | `CTSPool<CUtlVector<CPhysCollideVirtualMesh*, CUtlMemory<CPhysCollideVirtualMesh*, int>>>::Purge()` | identical |  |
| physics_virtualmesh | `CTSPool<CUtlVector<CPhysCollideVirtualMesh*, CUtlMemory<CPhysCollideVirtualMesh*, int>>>::~CTSPool()` | identical |  |
| physics_virtualmesh | `CPhysCollideVirtualMesh::GetAllLedges(IVP_U_BigVector<IVP_Compact_Ledge>&) const` | identical |  |
| physics_virtualmesh | `CPhysCollideVirtualMesh::GetMassCenter() const` | identical |  |
| physics_virtualmesh | `CPhysCollideVirtualMesh::GetSphereRadius() const` | identical |  |
| physics_virtualmesh | `CPhysCollideVirtualMesh::OutputDebugInfo() const` | identical |  |
| physics_virtualmesh | `CPhysCollideVirtualMesh::SerializeToBuffer(char*, bool) const` | identical |  |
| physics_virtualmesh | `CPhysCollideVirtualMesh::CreateSurfaceManager(short&) const` | identical |  |
| physics_virtualmesh | `CPhysCollideVirtualMesh::GetSerializationSize() const` | identical |  |
| physics_virtualmesh | `IVP_SurfaceManager_VirtualMesh::get_mass_center(IVP_U_Float_Point*) const` | identical |  |
| physics_virtualmesh | `IVP_SurfaceManager_VirtualMesh::get_single_convex() const` | identical |  |
| physics_virtualmesh | `IVP_SurfaceManager_VirtualMesh::get_radius_and_radius_dev_to_given_center(IVP_U_Float_Point const*, float*, float*) const` | identical |  |
| physics_virtualmesh | `VirtualMeshPSI()` | identical |  |
| physics_virtualmesh | `CDataManager<CMeshInstance, virtualmeshlist_t, CMeshInstance*, CThreadFastMutex>::GetRealSize(void*)` | identical |  |
| physics_virtualmesh | `CPhysCollide::SetOrthographicAreas(Vector const&)` | identical |  |
| physics_virtualmesh | `CPhysCollide::ComputeOrthographicAreas(float)` | identical |  |
| physics_virtualmesh | `CPhysCollideVirtualMesh::SetMassCenter(Vector const&)` | identical |  |
| physics_virtualmesh | `IVP_SurfaceManager_VirtualMesh::get_all_terminal_ledges(IVP_U_BigVector<IVP_Compact_Ledge>*)` | identical |  |
| physics_virtualmesh | `IVP_SurfaceManager_VirtualMesh::get_type()` | identical |  |
| physics_virtualmesh | `CTSPool<CUtlVector<CPhysCollideVirtualMesh*, CUtlMemory<CPhysCollideVirtualMesh*, int>>>::~CTSPool()` | identical |  |
| physics_virtualmesh | `CPhysCollide::GetCollideMap() const` | identical |  |
| physics_virtualmesh | `CPhysCollide::GetCompactSurface() const` | identical |  |
| physics_virtualmesh | `CPhysCollideVirtualMesh::GetVCollideIndex() const` | identical |  |
| physics_virtualmesh | `CPhysCollideVirtualMesh::GetOrthographicAreas() const` | identical |  |
| physics_virtualmesh | `IVP_SurfaceManager_VirtualMesh::get_rotation_inertia(IVP_U_Float_Point*) const` | identical |  |
| polyhedron | `_GLOBAL__I_a` | identical |  |
| polyhedron | `GetTempPolyhedron(unsigned short, unsigned short, unsigned short, unsigned short)` | identical |  |
| polyhedron | `CUtlMemory<unsigned char, int>::Grow(int)` | identical |  |
| polyhedron | `CUtlVector<unsigned char, CUtlMemory<unsigned char, int>>::~CUtlVector()` | identical |  |
| polyhedron | `CPolyhedron_AllocByNew::Allocate(unsigned short, unsigned short, unsigned short, unsigned short)` | identical |  |
| polyhedron | `CPolyhedron_AllocByNew::~CPolyhedron_AllocByNew()` | identical |  |
| polyhedron | `CPolyhedron_TempMemory::~CPolyhedron_TempMemory()` | identical |  |
| polyhedron | `CPolyhedron_AllocByNew::Release()` | identical |  |
| polyhedron | `CPolyhedron_AllocByNew::~CPolyhedron_AllocByNew()` | identical |  |
| polyhedron | `CPolyhedron_TempMemory::Release()` | identical |  |
| polyhedron | `CPolyhedron_TempMemory::~CPolyhedron_TempMemory()` | identical |  |
| prismatic_constraint | `hk_Prismatic_Constraint::step_constraint(hk_PSI_Info&, void*, float, float)` | identical |  |
| prismatic_constraint | `hk_Prismatic_Constraint::reset_linear_limit()` | identical |  |
| prismatic_constraint | `hk_Prismatic_Constraint::get_constraint_type()` | identical |  |
| prismatic_constraint | `hk_Prismatic_Constraint::init_prismatic_constraint(hk_Prismatic_BP const*, hk_Local_Constraint_System*)` | edited |  |
| prismatic_constraint | `hk_Prismatic_Constraint::setup_and_step_constraint(hk_PSI_Info&, void*, float, float)` | identical |  |
| prismatic_constraint | `hk_Prismatic_Constraint::set_motor(float, float)` | edited |  |
| prismatic_constraint | `hk_Prismatic_Constraint::hk_Prismatic_Constraint(hk_Environment*, hk_Prismatic_BP const*, hk_Rigid_Body*, hk_Rigid_Body*)` | identical |  |
| prismatic_constraint | `hk_Prismatic_Constraint::hk_Prismatic_Constraint(hk_Local_Constraint_System*, hk_Prismatic_BP const*, hk_Rigid_Body*, hk_Rigid_Body*)` | identical |  |
| prismatic_constraint | `hk_Prismatic_Constraint::~hk_Prismatic_Constraint()` | identical |  |
| prismatic_constraint | `hk_Constraint_Limit_Util::do_linear_limit(hk_PSI_Info&, hk_Rigid_Body*, hk_Rigid_Body*, hk_Vector3&, hk_Vector3&, hk_Vector3&, hk_Constraint_Limit&, f` | identical |  |
| prismatic_constraint | `hk_Prismatic_Constraint::init_constraint(void const*)` | identical |  |
| prismatic_constraint | `hk_Prismatic_Constraint::get_constraint_dof()` | identical |  |
| prismatic_constraint | `hk_Prismatic_Constraint::write_to_blueprint(hk_Prismatic_BP*)` | identical |  |
| prismatic_constraint | `hk_Prismatic_Constraint::get_vmq_storage_size()` | identical |  |
| prismatic_constraint | `hk_Prismatic_Constraint::~hk_Prismatic_Constraint()` | identical |  |
| pulley_constraint | `hk_Pulley_Constraint::step_constraint(hk_PSI_Info&, void*, float, float)` | differs | size retail 476 vs ours 478 |
| pulley_constraint | `hk_Pulley_Constraint::setup_and_step_constraint(hk_PSI_Info&, void*, float, float)` | differs | size retail 4627 vs ours 2885 |
| pulley_constraint | `hk_Pulley_Constraint::get_constraint_type()` | identical |  |
| pulley_constraint | `hk_Pulley_Constraint::hk_Pulley_Constraint(hk_Environment*, hk_Pulley_BP const*, hk_Rigid_Body*, hk_Rigid_Body*)` | identical |  |
| pulley_constraint | `hk_Pulley_Constraint::hk_Pulley_Constraint(hk_Local_Constraint_System*, hk_Pulley_BP const*, hk_Rigid_Body*, hk_Rigid_Body*)` | identical |  |
| pulley_constraint | `hk_Pulley_Constraint::~hk_Pulley_Constraint()` | identical |  |
| pulley_constraint | `hk_Pulley_Constraint::~hk_Pulley_Constraint()` | identical |  |
| pulley_constraint | `hk_Pulley_Constraint::init_constraint(void const*)` | identical |  |
| pulley_constraint | `hk_Pulley_Constraint::get_constraint_dof()` | identical |  |
| pulley_constraint | `hk_Pulley_Constraint::write_to_blueprint(hk_Pulley_BP*)` | identical |  |
| pulley_constraint | `hk_Pulley_Constraint::get_vmq_storage_size()` | identical |  |
| qhull | `qh_addpoint(double*, facetT*, unsigned int)` | identical |  |
| qhull | `qh_errexit2(int, facetT*, facetT*)` | identical |  |
| qhull | `qh_buildhull()` | identical |  |
| qhull | `qh_precision(char const*)` | identical |  |
| qhull | `qh_findhorizon(double*, facetT*, int*, int*)` | identical |  |
| qhull | `qh_buildtracing(double*, facetT*)` | identical |  |
| qhull | `qh_nextfurthest(facetT**)` | identical |  |
| qhull | `qh_partitionall(setT*, double*, int)` | identical |  |
| qhull | `qh_printsummary(__sFILE*)` | identical |  |
| qhull | `qh_partitionpoint(double*, facetT*)` | identical |  |
| qhull | `qh_partitionvisible(unsigned int, int*)` | identical |  |
| qhull | `qh_build_withrestart()` | edited |  |
| qhull | `qh_partitioncoplanar(double*, facetT*, double*)` | identical |  |
| qhull | `qh_qhull()` | identical |  |
| qhull_geom | `qh_findbest(double*, facetT*, unsigned int, unsigned int, unsigned int, double*, unsigned int*, int*)` | identical |  |
| qhull_geom | `qh_getangle(double*, double*)` | identical |  |
| qhull_geom | `qh_distplane(double*, facetT*, double*)` | identical |  |
| qhull_geom | `qh_gausselim(double**, int, int, unsigned int*, unsigned int*)` | identical |  |
| qhull_geom | `qh_getcenter(setT*)` | identical |  |
| qhull_geom | `qh_backnormal(double**, int, int, unsigned int, double*, unsigned int*)` | identical |  |
| qhull_geom | `qh_getcentrum(facetT*)` | identical |  |
| qhull_geom | `qh_normalize2(double*, int, unsigned int, double*, unsigned int*)` | identical |  |
| qhull_geom | `qh_findbestnew(double*, facetT*, double*, unsigned int*, int*)` | identical |  |
| qhull_geom | `qh_getdistance(facetT*, facetT*, double*, double*)` | identical |  |
| qhull_geom | `qh_projectpoint(double*, facetT*, double)` | identical |  |
| qhull_geom | `qh_setfacetplane(facetT*)` | identical |  |
| qhull_geom | `qh_sethyperplane_det(int, double**, double*, unsigned int, double*, double*, unsigned int*)` | identical |  |
| qhull_geom | `qh_sethyperplane_gauss(int, double**, double*, unsigned int, double*, double*, unsigned int*)` | identical |  |
| qhull_geom | `qh_normalize(double*, int, unsigned int)` | identical |  |
| qhull_geom2 | `qh_voronoi_center(int, setT*)` | differs | size retail 1929 vs ours 1913 |
| qhull_geom2 | `qh_divzero(double, double, double, unsigned int*)` | identical |  |
| qhull_geom2 | `qh_getarea(facetT*)` | identical |  |
| qhull_geom2 | `qh_detjoggle(double*, int, int)` | identical |  |
| qhull_geom2 | `qh_facetarea(facetT*)` | identical |  |
| qhull_geom2 | `qh_maxabsval(double*, int)` | identical |  |
| qhull_geom2 | `qh_scalelast(double*, int, int, double, double, double)` | identical |  |
| qhull_geom2 | `qh_detsimplex(double*, setT*, int, unsigned int*)` | identical |  |
| qhull_geom2 | `qh_maxsimplex(int, setT*, double*, int, setT**)` | identical |  |
| qhull_geom2 | `qh_outerinner(facetT*, double*, double*)` | identical |  |
| qhull_geom2 | `qh_scaleinput()` | identical |  |
| qhull_geom2 | `qh_determinant(double**, int, unsigned int*)` | identical |  |
| qhull_geom2 | `qh_detroundoff()` | identical |  |
| qhull_geom2 | `qh_facetcenter(setT*)` | identical |  |
| qhull_geom2 | `qh_joggleinput()` | identical |  |
| qhull_geom2 | `qh_printmatrix(__sFILE*, char const*, double**, int, int)` | identical |  |
| qhull_geom2 | `qh_printpoints(__sFILE*, char const*, setT*)` | identical |  |
| qhull_geom2 | `qh_rotateinput(double**)` | identical |  |
| qhull_geom2 | `qh_scalepoints(double*, int, int, double*, double*)` | identical |  |
| qhull_geom2 | `qh_setdelaunay(int, int, double*)` | identical |  |
| qhull_geom2 | `qh_findgooddist(double*, facetT*, double*, facetT**)` | identical |  |
| qhull_geom2 | `qh_gram_schmidt(int, double**)` | identical |  |
| qhull_geom2 | `qh_inthresholds(double*, double*)` | identical |  |
| qhull_geom2 | `qh_projectinput()` | identical |  |
| qhull_geom2 | `qh_randomfactor()` | identical |  |
| qhull_geom2 | `qh_randommatrix(double*, int, double**)` | identical |  |
| qhull_geom2 | `qh_rotatepoints(double*, int, int, double**)` | identical |  |
| qhull_geom2 | `qh_sethalfspace(int, double*, double**, double*, double*, double*)` | identical |  |
| qhull_geom2 | `qh_findbestsharp(double*, facetT**, double*, int*)` | identical |  |
| qhull_geom2 | `qh_orientoutside(facetT*)` | identical |  |
| qhull_geom2 | `qh_projectpoints(signed char*, int, double*, int, int, double*, int)` | identical |  |
| qhull_geom2 | `qh_sethalfspace_all(int, int, double*, double*)` | identical |  |
| qhull_geom2 | `qh_facetarea_simplex(int, double*, setT*, vertexT*, unsigned int, double*, double*)` | identical |  |
| qhull_geom2 | `qh_rand()` | identical |  |
| qhull_geom2 | `qh_srand(int)` | identical |  |
| qhull_geom2 | `qh_maxmin(double*, int, int)` | identical |  |
| qhull_geom2 | `qh_distnorm(int, double*, double*, double*)` | identical |  |
| qhull_geom2 | `qh_pointdist(double*, double*, int)` | identical |  |
| qhull_geom2 | `qh_crossproduct(int, double*, double*, double*)` | identical |  |
| qhull_global | `qh_freebuild(unsigned int)` | identical |  |
| qhull_global | `qh_freeqhull(unsigned int)` | identical |  |
| qhull_global | `qh_initflags(char*)` | identical |  |
| qhull_global | `qh_freebuffers()` | identical |  |
| qhull_global | `qh_initqhull_mem()` | identical |  |
| qhull_global | `qh_initthresholds(char*)` | identical |  |
| qhull_global | `qh_initqhull_start(__sFILE*, __sFILE*, __sFILE*)` | identical |  |
| qhull_global | `qh_initqhull_buffers()` | identical |  |
| qhull_global | `qh_initqhull_globals(double*, int, int, unsigned int)` | identical |  |
| qhull_global | `qh_init_B(double*, int, int, unsigned int)` | identical |  |
| qhull_global | `qh_option(char const*, int*, double*)` | edited |  |
| qhull_global | `qh_strtod(char const*, char**)` | identical |  |
| qhull_io | `qh_detvnorm(vertexT*, vertexT*, setT*, double*)` | identical |  |
| qhull_io | `qh_markkeep(facetT*)` | identical |  |
| qhull_io | `qh_printend(__sFILE*, int, facetT*, setT*, unsigned int)` | identical |  |
| qhull_io | `qh_detvridge(vertexT*)` | identical |  |
| qhull_io | `qh_detvridge3(vertexT*, vertexT*)` | identical |  |
| qhull_io | `qh_printbegin(__sFILE*, int, facetT*, setT*, unsigned int)` | identical |  |
| qhull_io | `qh_printridge(__sFILE*, ridgeT*)` | identical |  |
| qhull_io | `qh_printvnorm(__sFILE*, vertexT*, vertexT*, setT*, unsigned int)` | identical |  |
| qhull_io | `qh_countfacets(facetT*, setT*, unsigned int, int*, int*, int*, int*, int*)` | identical |  |
| qhull_io | `qh_eachvoronoi(__sFILE*, void (*)(__sFILE*, vertexT*, vertexT*, setT*, unsigned int), vertexT*, unsigned int, qh_RIDGE, unsigned int)` | identical |  |
| qhull_io | `qh_facet2point(facetT*, double**, double**, double*)` | identical |  |
| qhull_io | `qh_markvoronoi(facetT*, setT*, unsigned int, unsigned int*, int*)` | identical |  |
| qhull_io | `qh_printafacet(__sFILE*, int, facetT*, unsigned int)` | identical |  |
| qhull_io | `qh_printcenter(__sFILE*, int, char const*, facetT*)` | identical |  |
| qhull_io | `qh_printfacets(__sFILE*, int, facetT*, setT*, unsigned int)` | identical |  |
| qhull_io | `qh_printpoint3(__sFILE*, double*)` | identical |  |
| qhull_io | `qh_printvertex(__sFILE*, vertexT*)` | identical |  |
| qhull_io | `qh_printvridge(__sFILE*, vertexT*, vertexT*, setT*, unsigned int)` | identical |  |
| qhull_io | `qh_setfeasible(int)` | identical |  |
| qhull_io | `qh_printcentrum(__sFILE*, facetT*, double)` | identical |  |
| qhull_io | `qh_printspheres(__sFILE*, setT*, double)` | identical |  |
| qhull_io | `qh_printvoronoi(__sFILE*, int, facetT*, setT*, unsigned int)` | identical |  |
| qhull_io | `qh_facetvertices(facetT*, setT*, unsigned int)` | identical |  |
| qhull_io | `qh_printend4geom(__sFILE*, facetT*, int*, unsigned int)` | identical |  |
| qhull_io | `qh_printextremes(__sFILE*, facetT*, setT*, int)` | identical |  |
| qhull_io | `qh_printvdiagram(__sFILE*, int, facetT*, setT*, unsigned int)` | identical |  |
| qhull_io | `qh_printline3geom(__sFILE*, double*, double*, double*)` | identical |  |
| qhull_io | `qh_printpointvect(__sFILE*, double*, double*, double*, double, double*)` | identical |  |
| qhull_io | `qh_produce_output()` | identical |  |
| qhull_io | `qh_printextremes_d(__sFILE*, facetT*, setT*, int)` | identical |  |
| qhull_io | `qh_printfacet2geom(__sFILE*, facetT*, double*)` | identical |  |
| qhull_io | `qh_printfacet2math(__sFILE*, facetT*, int)` | identical |  |
| qhull_io | `qh_printfacet3math(__sFILE*, facetT*, int)` | identical |  |
| qhull_io | `qh_printpoints_out(__sFILE*, facetT*, setT*, int)` | identical |  |
| qhull_io | `qh_printvneighbors(__sFILE*, facetT*, setT*, unsigned int)` | identical |  |
| qhull_io | `qh_printextremes_2d(__sFILE*, facetT*, setT*, int)` | identical |  |
| qhull_io | `qh_printfacetheader(__sFILE*, facetT*)` | identical |  |
| qhull_io | `qh_printfacetridges(__sFILE*, facetT*)` | identical |  |
| qhull_io | `qh_printfacet3vertex(__sFILE*, facetT*, int)` | identical |  |
| qhull_io | `qh_printneighborhood(__sFILE*, int, facetT*, facetT*, unsigned int)` | identical |  |
| qhull_io | `qh_printhelp_singular(__sFILE*)` | identical |  |
| qhull_io | `qh_printhelp_degenerate(__sFILE*)` | identical |  |
| qhull_io | `qh_order_vertexneighbors(vertexT*)` | identical |  |
| qhull_io | `qh_printfacet2geom_points(__sFILE*, double*, double*, facetT*, double, double*)` | identical |  |
| qhull_io | `qh_printfacet3geom_points(__sFILE*, setT*, facetT*, double, double*)` | identical |  |
| qhull_io | `qh_printfacet3geom_simplicial(__sFILE*, facetT*, double*)` | identical |  |
| qhull_io | `qh_printfacet4geom_simplicial(__sFILE*, facetT*, double*)` | identical |  |
| qhull_io | `qh_printhyperplaneintersection(__sFILE*, facetT*, facetT*, setT*, double*)` | identical |  |
| qhull_io | `qh_printfacetNvertex_simplicial(__sFILE*, facetT*, int)` | identical |  |
| qhull_io | `qh_printfacet3geom_nonsimplicial(__sFILE*, facetT*, double*)` | identical |  |
| qhull_io | `qh_printfacet4geom_nonsimplicial(__sFILE*, facetT*, double*)` | identical |  |
| qhull_io | `qh_printfacetNvertex_nonsimplicial(__sFILE*, facetT*, int, int)` | identical |  |
| qhull_io | `qh_printfacet(__sFILE*, facetT*)` | identical |  |
| qhull_io | `qh_compare_facetarea(void const*, void const*)` | identical |  |
| qhull_io | `qh_compare_facetmerge(void const*, void const*)` | identical |  |
| qhull_io | `qh_compare_facetvisit(void const*, void const*)` | identical |  |
| qhull_mem | `qh_memfree(void*, int)` | identical |  |
| qhull_mem | `qh_meminit(__sFILE*)` | identical |  |
| qhull_mem | `qh_memsize(int)` | identical |  |
| qhull_mem | `qh_memalloc(int)` | identical |  |
| qhull_mem | `qh_memsetup()` | identical |  |
| qhull_mem | `qh_memfreeshort(int*, int*)` | identical |  |
| qhull_mem | `qh_memstatistics(__sFILE*)` | identical |  |
| qhull_mem | `qh_meminitbuffers(int, int, int, int, int)` | identical |  |
| qhull_mem | `qh_intcompare(void const*, void const*)` | identical |  |
| qhull_merge | `qh_premerge(vertexT*, double, double)` | identical |  |
| qhull_merge | `qh_checkzero(unsigned int)` | identical |  |
| qhull_merge | `qh_postmerge(char const*, double, double, unsigned int)` | identical |  |
| qhull_merge | `qh_all_merges(unsigned int, unsigned int)` | identical |  |
| qhull_merge | `qh_makeridges(facetT*)` | identical |  |
| qhull_merge | `qh_mergecycle(facetT*, facetT*)` | identical |  |
| qhull_merge | `qh_mergefacet(facetT*, facetT*, double*, double*, unsigned int)` | identical |  |
| qhull_merge | `qh_tracemerge(facetT*, facetT*)` | identical |  |
| qhull_merge | `qh_getmergeset(facetT*)` | identical |  |
| qhull_merge | `qh_mergeridges(facetT*, facetT*)` | identical |  |
| qhull_merge | `qh_basevertices(facetT*)` | identical |  |
| qhull_merge | `qh_forcedmerges(unsigned int*)` | identical |  |
| qhull_merge | `qh_mergefacet2d(facetT*, facetT*)` | identical |  |
| qhull_merge | `qh_mergesimplex(facetT*, facetT*, unsigned int)` | identical |  |
| qhull_merge | `qh_renamevertex(vertexT*, vertexT*, setT*, facetT*, facetT*)` | identical |  |
| qhull_merge | `qh_updatetested(facetT*, facetT*)` | identical |  |
| qhull_merge | `qh_vertexridges(vertexT*)` | identical |  |
| qhull_merge | `qh_flippedmerges(facetT*, unsigned int*)` | identical |  |
| qhull_merge | `qh_mergevertices(setT*, setT**)` | identical |  |
| qhull_merge | `qh_appendmergeset(facetT*, facetT*, mergeType, double*)` | identical |  |
| qhull_merge | `qh_find_newvertex(vertexT*, setT*, setT*)` | identical |  |
| qhull_merge | `qh_hashridge_find(setT*, int, ridgeT*, vertexT*, vertexT*, int*)` | identical |  |
| qhull_merge | `qh_mark_dupridges(facetT*)` | identical |  |
| qhull_merge | `qh_mergecycle_all(facetT*, unsigned int*)` | identical |  |
| qhull_merge | `qh_mergeneighbors(facetT*, facetT*)` | identical |  |
| qhull_merge | `qh_reducevertices()` | identical |  |
| qhull_merge | `qh_maydropneighbor(facetT*)` | identical |  |
| qhull_merge | `qh_merge_nonconvex(facetT*, facetT*, mergeType)` | identical |  |
| qhull_merge | `qh_test_vneighbors()` | identical |  |
| qhull_merge | `qh_findbestneighbor(facetT*, double*, double*, double*)` | identical |  |
| qhull_merge | `qh_redundant_vertex(vertexT*)` | identical |  |
| qhull_merge | `qh_test_appendmerge(facetT*, facetT*)` | identical |  |
| qhull_merge | `qh_mergecycle_facets(facetT*, facetT*)` | identical |  |
| qhull_merge | `qh_mergecycle_ridges(facetT*, facetT*)` | identical |  |
| qhull_merge | `qh_renameridgevertex(ridgeT*, vertexT*, vertexT*)` | identical |  |
| qhull_merge | `qh_getmergeset_initial(facetT*)` | identical |  |
| qhull_merge | `qh_rename_sharedvertex(vertexT*, facetT*)` | identical |  |
| qhull_merge | `qh_merge_degenredundant()` | identical |  |
| qhull_merge | `qh_mergecycle_neighbors(facetT*, facetT*)` | identical |  |
| qhull_merge | `qh_remove_extravertices(facetT*)` | identical |  |
| qhull_merge | `qh_degen_redundant_facet(facetT*)` | identical |  |
| qhull_merge | `qh_mergecycle_vneighbors(facetT*, facetT*)` | identical |  |
| qhull_merge | `qh_mergevertex_neighbors(facetT*, facetT*)` | identical |  |
| qhull_merge | `qh_neighbor_intersections(vertexT*)` | identical |  |
| qhull_merge | `qh_degen_redundant_neighbors(facetT*, facetT*)` | identical |  |
| qhull_merge | `qh_compareangle(void const*, void const*)` | identical |  |
| qhull_merge | `qh_comparemerge(void const*, void const*)` | identical |  |
| qhull_merge | `qh_comparevisit(void const*, void const*)` | identical |  |
| qhull_poly | `qh_pointid(double*)` | identical |  |
| qhull_poly | `qh_delfacet(facetT*)` | identical |  |
| qhull_poly | `qh_newfacet()` | identical |  |
| qhull_poly | `qh_newridge()` | identical |  |
| qhull_poly | `qh_appendfacet(facetT*)` | identical |  |
| qhull_poly | `qh_removefacet(facetT*)` | identical |  |
| qhull_poly | `qh_appendvertex(vertexT*)` | identical |  |
| qhull_poly | `qh_checkflipped(facetT*, double*, unsigned int)` | identical |  |
| qhull_poly | `qh_makenewfacet(setT*, unsigned int, facetT*)` | identical |  |
| qhull_poly | `qh_removevertex(vertexT*)` | identical |  |
| qhull_poly | `qh_deletevisible()` | identical |  |
| qhull_poly | `qh_makenewplanes()` | identical |  |
| qhull_poly | `qh_matchneighbor(facetT*, int, int, int*)` | identical |  |
| qhull_poly | `qh_matchvertices(int, setT*, int, setT*, int*, unsigned int*)` | identical |  |
| qhull_poly | `qh_facetintersect(facetT*, facetT*, int*, int*, int)` | identical |  |
| qhull_poly | `qh_matchnewfacets()` | identical |  |
| qhull_poly | `qh_updatevertices()` | identical |  |
| qhull_poly | `qh_attachnewfacets()` | identical |  |
| qhull_poly | `qh_makenew_simplicial(facetT*, vertexT*, int*)` | identical |  |
| qhull_poly | `qh_makenew_nonsimplicial(facetT*, vertexT*, int*)` | identical |  |
| qhull_poly | `qh_gethash(int, setT*, int, int, void*)` | identical |  |
| qhull_poly2 | `qh_delridge(ridgeT*)` | identical |  |
| qhull_poly2 | `qh_findgood(facetT*, int)` | identical |  |
| qhull_poly2 | `qh_delvertex(vertexT*)` | identical |  |
| qhull_poly2 | `qh_initbuild()` | identical |  |
| qhull_poly2 | `qh_newvertex(double*)` | identical |  |
| qhull_poly2 | `qh_point_add(setT*, double*, void*)` | identical |  |
| qhull_poly2 | `qh_checkfacet(facetT*, unsigned int, unsigned int*)` | identical |  |
| qhull_poly2 | `qh_nearvertex(facetT*, double*, double*)` | identical |  |
| qhull_poly2 | `qh_pointfacet()` | identical |  |
| qhull_poly2 | `qh_printlists()` | identical |  |
| qhull_poly2 | `qh_resetlists(unsigned int)` | identical |  |
| qhull_poly2 | `qh_checkconvex(facetT*, int)` | identical |  |
| qhull_poly2 | `qh_checkvertex(vertexT*)` | identical |  |
| qhull_poly2 | `qh_furthestout(facetT*)` | identical |  |
| qhull_poly2 | `qh_initialhull(setT*)` | identical |  |
| qhull_poly2 | `qh_outcoplanar()` | identical |  |
| qhull_poly2 | `qh_pointvertex()` | identical |  |
| qhull_poly2 | `qh_check_maxout()` | identical |  |
| qhull_poly2 | `qh_checkpolygon(facetT*)` | identical |  |
| qhull_poly2 | `qh_clearcenters(qh_CENTER)` | identical |  |
| qhull_poly2 | `qh_facet3vertex(facetT*)` | identical |  |
| qhull_poly2 | `qh_findgood_all(facetT*)` | identical |  |
| qhull_poly2 | `qh_furthestnext()` | identical |  |
| qhull_poly2 | `qh_infiniteloop(facetT*)` | identical |  |
| qhull_poly2 | `qh_nearcoplanar()` | identical |  |
| qhull_poly2 | `qh_newhashtable(int)` | identical |  |
| qhull_poly2 | `qh_prependfacet(facetT*, facetT**)` | identical |  |
| qhull_poly2 | `qh_createsimplex(setT*)` | identical |  |
| qhull_poly2 | `qh_makenewfacets(double*)` | identical |  |
| qhull_poly2 | `qh_printhashtable(__sFILE*)` | identical |  |
| qhull_poly2 | `qh_initialvertices(int, setT*, double*, int)` | identical |  |
| qhull_poly2 | `qh_matchduplicates(facetT*, int, int, int*)` | identical |  |
| qhull_poly2 | `qh_vertexintersect(setT**, setT*)` | identical |  |
| qhull_poly2 | `qh_vertexneighbors()` | identical |  |
| qhull_poly2 | `qh_checkflipped_all(facetT*)` | identical |  |
| qhull_poly2 | `qh_vertexintersect_new(setT*, setT*)` | identical |  |
| qhull_poly2 | `qh_addhash(void*, setT*, int, unsigned int)` | identical |  |
| qhull_poly2 | `qh_nextridge3d(ridgeT*, facetT*, vertexT**)` | identical |  |
| qhull_qset | `qh_setcopy(setT*, int)` | identical |  |
| qhull_qset | `qh_setfree(setT**)` | identical |  |
| qhull_qset | `qh_setsize(setT*)` | identical |  |
| qhull_qset | `qh_settemp(int)` | identical |  |
| qhull_qset | `qh_setzero(setT*, int, int)` | identical |  |
| qhull_qset | `qh_setcheck(setT*, char const*, int)` | identical |  |
| qhull_qset | `qh_setequal(setT*, setT*)` | identical |  |
| qhull_qset | `qh_setprint(__sFILE*, char const*, setT*)` | identical |  |
| qhull_qset | `qh_setaddnth(setT**, int, void*)` | identical |  |
| qhull_qset | `qh_setdelnth(setT*, int)` | identical |  |
| qhull_qset | `qh_setlarger(setT**)` | identical |  |
| qhull_qset | `qh_setcompact(setT*)` | identical |  |
| qhull_qset | `qh_setreplace(setT*, void*, void*)` | identical |  |
| qhull_qset | `qh_settemppop()` | identical |  |
| qhull_qset | `qh_setfreelong(setT**)` | identical |  |
| qhull_qset | `qh_settempfree(setT**)` | identical |  |
| qhull_qset | `qh_settemppush(setT*)` | identical |  |
| qhull_qset | `qh_settruncate(setT*, int)` | identical |  |
| qhull_qset | `qh_setappend_set(setT**, setT*)` | identical |  |
| qhull_qset | `qh_setdelnthsorted(setT*, int)` | identical |  |
| qhull_qset | `qh_settempfree_all()` | identical |  |
| qhull_qset | `qh_setnew_delnthsorted(setT*, int, int, int)` | identical |  |
| qhull_qset | `qh_setnew(int)` | identical |  |
| qhull_qset | `qh_setlast(setT*)` | identical |  |
| qhull_qset | `qh_setindex(setT*, void*)` | identical |  |
| qhull_qset | `qh_setappend(setT**, void*)` | identical |  |
| qhull_qset | `qh_setunique(setT**, void*)` | identical |  |
| qhull_qset | `qh_setdellast(setT*)` | identical |  |
| qhull_qset | `qh_setdelsorted(setT*, void*)` | identical |  |
| qhull_qset | `qh_setequal_skip(setT*, int, setT*, int)` | identical |  |
| qhull_qset | `qh_setequal_except(setT*, void*, setT*, void*)` | identical |  |
| qhull_qset | `qh_setappend2ndlast(setT**, void*)` | identical |  |
| qhull_qset | `qh_setin(setT*, void*)` | identical |  |
| qhull_qset | `qh_setdel(setT*, void*)` | identical |  |
| qhull_stat | `qh_allstatA()` | identical |  |
| qhull_stat | `qh_allstatB()` | identical |  |
| qhull_stat | `qh_allstatC()` | identical |  |
| qhull_stat | `qh_allstatD()` | identical |  |
| qhull_stat | `qh_allstatE()` | identical |  |
| qhull_stat | `qh_allstatF()` | identical |  |
| qhull_stat | `qh_allstatG()` | identical |  |
| qhull_stat | `qh_allstatH()` | identical |  |
| qhull_stat | `qh_allstatI()` | identical |  |
| qhull_stat | `qh_printstats(__sFILE*, int, int*)` | identical |  |
| qhull_stat | `qh_allstatistics()` | identical |  |
| qhull_stat | `qh_initstatistics()` | identical |  |
| qhull_stat | `qh_printstatlevel(__sFILE*, int, int)` | identical |  |
| qhull_stat | `qh_printstatistics(__sFILE*, char const*)` | identical |  |
| qhull_stat | `qh_collectstatistics()` | identical |  |
| qhull_stat | `qh_printallstatistics(__sFILE*, char const*)` | identical |  |
| qhull_stat | `qh_freestatistics()` | identical |  |
| qhull_user | `qh_errexit(int, facetT*, ridgeT*)` | identical |  |
| qhull_user | `qh_errprint(char const*, facetT*, facetT*, ridgeT*, vertexT*)` | identical |  |
| qhull_user | `qh_new_qhull(int, int, double*, unsigned int, char*, __sFILE*, __sFILE*)` | identical |  |
| qhull_user | `qh_printfacetlist(facetT*, setT*, unsigned int)` | identical |  |
| qhull_user | `qh_user_memsizes()` | identical |  |
| quaternion | `hk_Quaternion::set(hk_Rotation const&)` | identical |  |
| ragdoll_constraint | `hk_Ragdoll_Constraint::setup_and_step_constraint(hk_PSI_Info&, void*, float, float)` | differs | size retail 5888 vs ours 5844 |
| ragdoll_constraint | `hk_Ragdoll_Constraint::step_constraint(hk_PSI_Info&, void*, float, float)` | identical |  |
| ragdoll_constraint | `hk_Ragdoll_Constraint::update_friction(float)` | edited |  |
| ragdoll_constraint | `hk_Ragdoll_Constraint::write_to_blueprint(hk_Ragdoll_Constraint_BP*)` | edited |  |
| ragdoll_constraint | `hk_Ragdoll_Constraint::get_constraint_type()` | identical |  |
| ragdoll_constraint | `hk_Ragdoll_Constraint::init_ragdoll_constraint(hk_Ragdoll_Constraint_BP const*, hk_Local_Constraint_System*)` | edited |  |
| ragdoll_constraint | `hk_Ragdoll_Constraint::hk_Ragdoll_Constraint(hk_Environment*, hk_Ragdoll_Constraint_BP const*, hk_Rigid_Body*, hk_Rigid_Body*)` | identical |  |
| ragdoll_constraint | `hk_Ragdoll_Constraint::hk_Ragdoll_Constraint(hk_Local_Constraint_System*, hk_Ragdoll_Constraint_BP const*, hk_Rigid_Body*, hk_Rigid_Body*)` | identical |  |
| ragdoll_constraint | `hk_Ragdoll_Constraint::~hk_Ragdoll_Constraint()` | identical |  |
| ragdoll_constraint | `hk_Ragdoll_Constraint::~hk_Ragdoll_Constraint()` | identical |  |
| ragdoll_constraint | `hk_Constraint_Limit_Util::do_angular_plane_limit(hk_PSI_Info&, hk_Rigid_Body*, hk_Rigid_Body*, hk_Vector3&, hk_Vector3&, float, hk_Constraint_Limit&, ` | identical |  |
| ragdoll_constraint | `hk_Ragdoll_Constraint::init_constraint(void const*)` | identical |  |
| ragdoll_constraint | `hk_Ragdoll_Constraint::update_transforms(hk_Transform const&, hk_Transform const&)` | identical |  |
| ragdoll_constraint | `hk_Ragdoll_Constraint::get_constraint_dof()` | identical |  |
| ragdoll_constraint | `hk_Ragdoll_Constraint::get_vmq_storage_size()` | identical |  |
| ragdoll_constraint | `hk_Ragdoll_Constraint::hk_Ragdoll_Constraint(hk_Environment*, hk_Ragdoll_Constraint_BP const*, hk_Rigid_Body*, hk_Rigid_Body*)` | identical |  |
| ragdoll_constraint | `hk_Ragdoll_Constraint::hk_Ragdoll_Constraint(hk_Local_Constraint_System*, hk_Ragdoll_Constraint_BP const*, hk_Rigid_Body*, hk_Rigid_Body*)` | identical |  |
| ragdoll_constraint_bp_builder | `hk_Ragdoll_Constraint_BP_Builder::initialize_from_limited_ball_socket_bp(hk_Limited_Ball_Socket_BP const*, hk_Rigid_Body*, hk_Rigid_Body*)` | edited |  |
| rigid_body_binary_effector | `hk_Rigid_Body_Binary_EF::hk_Rigid_Body_Binary_EF(hk_Environment*, hk_Rigid_Body*, hk_Rigid_Body*, IVP_CONTROLLER_PRIORITY)` | identical |  |
| rigid_body_core | `hk_Rigid_Body_Core::apply_impulses(hk_Core_VMQ_Input&, float const*)` | differs | size retail 252 vs ours 251 |
| rigid_body_core | `hk_Rigid_Body_Core::add_to_mass_matrix_inv(hk_Core_VMQ_Input&, hk_Dense_Matrix&, float*)` | identical |  |
| rotation | `hk_Rotation::set(hk_Quaternion const&)` | identical |  |
| sse | `_GLOBAL__I_a` | identical |  |
| sse | `_SSE_RSqrtAccurate(float)` | identical |  |
| sse | `_SSE_VectorNormalizeFast(Vector&)` | identical |  |
| sse | `_SSE_RSqrtFast(float)` | identical |  |
| sse | `_SSE_InvRSquared(float const*)` | identical |  |
| sse | `_SSE_Sqrt(float)` | identical |  |
| sseconst | `FourVectors::RotateManyBy(FourVectors*, unsigned int, matrix3x4_t const&)` | identical |  |
| stiff_spring_constraint | `hk_Stiff_Spring_Constraint::step_constraint(hk_PSI_Info&, void*, float, float)` | identical |  |
| stiff_spring_constraint | `hk_Stiff_Spring_Constraint::get_constraint_type()` | identical |  |
| stiff_spring_constraint | `hk_Stiff_Spring_Constraint::setup_and_step_constraint(hk_PSI_Info&, void*, float, float)` | edited |  |
| stiff_spring_constraint | `hk_Stiff_Spring_Constraint::hk_Stiff_Spring_Constraint(hk_Environment*, hk_Stiff_Spring_BP const*, hk_Rigid_Body*, hk_Rigid_Body*)` | identical |  |
| stiff_spring_constraint | `hk_Stiff_Spring_Constraint::hk_Stiff_Spring_Constraint(hk_Local_Constraint_System*, hk_Stiff_Spring_BP const*, hk_Rigid_Body*, hk_Rigid_Body*)` | identical |  |
| stiff_spring_constraint | `hk_Stiff_Spring_Constraint::~hk_Stiff_Spring_Constraint()` | identical |  |
| stiff_spring_constraint | `hk_Stiff_Spring_Constraint::~hk_Stiff_Spring_Constraint()` | identical |  |
| stiff_spring_constraint | `hk_Stiff_Spring_Constraint::init_constraint(void const*)` | identical |  |
| stiff_spring_constraint | `hk_Stiff_Spring_Constraint::get_constraint_dof()` | identical |  |
| stiff_spring_constraint | `hk_Stiff_Spring_Constraint::write_to_blueprint(hk_Stiff_Spring_BP*)` | identical |  |
| stiff_spring_constraint | `hk_Stiff_Spring_Constraint::get_vmq_storage_size()` | identical |  |
| strtools | `V_vsnprintfRet(char*, int, char const*, char*, bool*)` | missing |  |
| strtools | `V_snprintf(char*, int, char const*, ...)` | identical |  |
| strtools | `V_strlower(char*)` | identical |  |
| strtools | `V_stricmp(char const*, char const*)` | identical |  |
| strtools | `V_strncpy(char*, char const*, int)` | identical |  |
| tier1 | `ConnectTier1Libraries(void* (**)(char const*, int*), int)` | identical |  |
| tier1 | `DisconnectTier1Libraries()` | identical |  |
| trace | `simplex_t::ClipRayToTriangle(Vector const&, float)` | differs | size retail 1437 vs ours 1404 |
| trace | `_GLOBAL__I_a` | identical |  |
| trace | `InitLeafmap(IVP_Compact_Ledge*, leafmap_t*)` | identical |  |
| trace | `TraceGetAABB_r(Vector*, Vector*, IVP_Compact_Ledgetree_Node const*, CTraceIVP&)` | identical |  |
| trace | `CUtlMemory<IVP_Compact_Ledgetree_Node const*, int>::Grow(int)` | identical |  |
| trace | `CUtlVector<IVP_Compact_Ledgetree_Node const*, CUtlMemoryFixedGrowable<IVP_Compact_Ledgetree_Node const*, 64ul, int>>::InsertBefore(int)` | identical |  |
| trace | `CTraceSolver::SweepSingleConvex()` | identical |  |
| trace | `CTraceSolver::SolveMeshIntersection(simplex_t&)` | identical |  |
| trace | `CTraceSolver::DoSweep()` | identical |  |
| trace | `CPhysicsTrace::SweepBoxIVP(Ray_t const&, unsigned int, IConvexInfo*, CPhysCollide const*, Vector const&, QAngle const&, CGameTrace*)` | identical |  |
| trace | `CPhysicsTrace::SweepBoxIVP(Vector const&, Vector const&, Vector const&, Vector const&, CPhysCollide const*, Vector const&, QAngle const&, CGameTrace*)` | identical |  |
| trace | `CPhysicsTrace::IsBoxIntersectingCone(Vector const&, Vector const&, truncatedcone_t const&)` | identical |  |
| trace | `CPhysicsTrace::GetAABB(Vector*, Vector*, CPhysCollide const*, Vector const&, QAngle const&)` | identical |  |
| trace | `CPhysicsTrace::SweepIVP(Vector const&, Vector const&, CPhysCollide const*, QAngle const&, CPhysCollide const*, Vector const&, QAngle const&, CGameTrac` | identical |  |
| trace | `CPhysicsTrace::GetExtent(CPhysCollide const*, Vector const&, QAngle const&, Vector const&)` | identical |  |
| trace | `CTraceSolverSweptObject::SweepLedgeTree_r(IVP_Compact_Ledgetree_Node const*)` | identical |  |
| trace | `CTraceSolverSweptObject::DoSweep()` | identical |  |
| trace | `CTraceSolverSweptObject::InitOSRay()` | identical |  |
| trace | `CTSPool<CVisitHash>::~CTSPool()` | identical |  |
| trace | `CTraceIVP::BuildLeafmapCache(leafmap_t const*)` | identical |  |
| trace | `CTraceIVP::BuildLeafmapCacheRLE(leafmap_t const*)` | identical |  |
| trace | `CTraceIVP::SetLedge(IVP_Compact_Ledge const*)` | identical |  |
| trace | `CTraceIVP::CTraceIVP(CPhysCollide const*, Vector const&, QAngle const&)` | identical |  |
| trace | `CTraceIVP::~CTraceIVP()` | identical |  |
| trace | `CTraceRay::CTraceRay(Ray_t const&, Vector const&)` | identical |  |
| trace | `simplex_t::SolveGJKSet(simplexvert_t const&, Vector*)` | identical |  |
| trace | `simplex_t::SolveVoronoiRegion3(simplexvert_t const&, Vector*)` | identical |  |
| trace | `simplex_t::SolveVoronoiRegion4(simplexvert_t const&, Vector*)` | identical |  |
| trace | `simplex_t::ClipRayToTetrahedron(Vector const&)` | identical |  |
| trace | `simplex_t::ClipRayToTetrahedronBase(Vector const&)` | identical |  |
| trace | `CTraceCone::SupportMap(Vector const&, Vector*) const` | identical |  |
| trace | `CTraceIVP::SupportMap(Vector const&, Vector*) const` | identical |  |
| trace | `CTraceIVP::GetVertByIndex(int) const` | identical |  |
| trace | `TraceGetExtent_r(IVP_Compact_Ledgetree_Node const*, CTraceIVP&, Vector const&, float&, Vector&)` | identical |  |
| trace | `CPhysicsTrace::CPhysicsTrace()` | identical |  |
| trace | `CPhysicsTrace::~CPhysicsTrace()` | identical |  |
| trace | `CDefConvexInfo::GetContents(int)` | identical |  |
| trace | `CTSPool<CVisitHash>::~CTSPool()` | identical |  |
| trace | `simplex_t::SolveVoronoiRegion2(simplexvert_t const&, Vector*)` | identical |  |
| trace | `CTraceAABB::SupportMap(Vector const&, Vector*) const` | identical |  |
| trace | `CTraceAABB::GetVertByIndex(int) const` | identical |  |
| trace | `CTraceAABB::Radius() const` | identical |  |
| trace | `CTraceCone::GetVertByIndex(int) const` | identical |  |
| trace | `CTraceCone::Radius() const` | identical |  |
| trace | `CTraceIVP::Radius() const` | identical |  |
| transform | `hk_Transform::set_identity_transform()` | identical |  |
| utlbuffer | `_GLOBAL__I_a` | identical |  |
| utlbuffer | `CUtlBuffer::PutOverflow(int)` | identical |  |
| utlbuffer | `CUtlBuffer::SetExternalBuffer(void*, int, int, int)` | identical |  |
| utlbuffer | `CUtlBuffer::Put(void const*, int)` | identical |  |
| utlbuffer | `CUtlBuffer::CUtlBuffer(int, int, int)` | identical |  |
| utlbuffer | `CUtlCharConversion::FindConversion(char const*, int*)` | identical |  |
| utlbuffer | `CUtlCStringConversion::CUtlCStringConversion(char, char const*, int, CUtlCharConversion::ConversionArray_t*)` | identical |  |
| utlbuffer | `CUtlBuffer::GetOverflow(int)` | identical |  |
| utlbuffer | `CUtlBuffer::AddNullTermination()` | identical |  |
| utlbuffer | `CUtlBuffer::CUtlBuffer(int, int, int)` | identical |  |
| utlbuffer | `CUtlNoEscConversion::FindConversion(char const*, int*)` | identical |  |
| utlbuffer | `CUtlCStringConversion::FindConversion(char const*, int*)` | identical |  |
| utlsymbol | `_GLOBAL__I_a` | identical |  |
| utlsymbol | `CUtlMemory<UtlRBTreeNode_t<CUtlSymbolTable::CStringPoolIndex, unsigned short>, unsigned short>::Grow(int)` | identical |  |
| utlsymbol | `CUtlMemory<CUtlSymbolTable::StringPool_t*, int>::Grow(int)` | identical |  |
| utlsymbol | `CUtlRBTree<CUtlSymbolTable::CStringPoolIndex, unsigned short, CUtlSymbolTable::CLess, CUtlMemory<UtlRBTreeNode_t<CUtlSymbolTable::CStringPoolIndex, un` | identical |  |
| utlsymbol | `CUtlRBTree<CUtlSymbolTable::CStringPoolIndex, unsigned short, CUtlSymbolTable::CLess, CUtlMemory<UtlRBTreeNode_t<CUtlSymbolTable::CStringPoolIndex, un` | identical |  |
| utlsymbol | `CUtlRBTree<CUtlSymbolTable::CStringPoolIndex, unsigned short, CUtlSymbolTable::CLess, CUtlMemory<UtlRBTreeNode_t<CUtlSymbolTable::CStringPoolIndex, un` | identical |  |
| utlsymbol | `CUtlRBTree<CUtlSymbolTable::CStringPoolIndex, unsigned short, CUtlSymbolTable::CLess, CUtlMemory<UtlRBTreeNode_t<CUtlSymbolTable::CStringPoolIndex, un` | identical |  |
| utlsymbol | `CUtlRBTree<CUtlSymbolTable::CStringPoolIndex, unsigned short, CUtlSymbolTable::CLess, CUtlMemory<UtlRBTreeNode_t<CUtlSymbolTable::CStringPoolIndex, un` | identical |  |
| utlsymbol | `CUtlRBTree<CUtlSymbolTable::CStringPoolIndex, unsigned short, CUtlSymbolTable::CLess, CUtlMemory<UtlRBTreeNode_t<CUtlSymbolTable::CStringPoolIndex, un` | identical |  |
| utlsymbol | `CThreadLocalPtr<char>::~CThreadLocalPtr()` | identical |  |
| utlsymbol | `CUtlSymbolTable::AddString(char const*)` | identical |  |
| utlsymbol | `CUtlSymbolTable::RemoveAll()` | identical |  |
| utlsymbol | `CUtlSymbolTable::CUtlSymbolTable(int, int, bool)` | identical |  |
| utlsymbol | `CUtlSymbolTable::~CUtlSymbolTable()` | identical |  |
| utlsymbol | `CCleanupUtlSymbolTable::~CCleanupUtlSymbolTable()` | identical |  |
| utlsymbol | `CUtlRBTree<CUtlSymbolTable::CStringPoolIndex, unsigned short, CUtlSymbolTable::CLess, CUtlMemory<UtlRBTreeNode_t<CUtlSymbolTable::CStringPoolIndex, un` | identical |  |
| utlsymbol | `CUtlRBTree<CUtlSymbolTable::CStringPoolIndex, unsigned short, CUtlSymbolTable::CLess, CUtlMemory<UtlRBTreeNode_t<CUtlSymbolTable::CStringPoolIndex, un` | identical |  |
| utlsymbol | `CUtlRBTree<CUtlSymbolTable::CStringPoolIndex, unsigned short, CUtlSymbolTable::CLess, CUtlMemory<UtlRBTreeNode_t<CUtlSymbolTable::CStringPoolIndex, un` | identical |  |
| utlsymbol | `CUtlSymbolTable::Find(char const*) const` | identical |  |
| utlsymbol | `CUtlSymbolTable::CLess::operator()(CUtlSymbolTable::CStringPoolIndex const&, CUtlSymbolTable::CStringPoolIndex const&) const` | identical |  |
| utlsymbol | `CUtlSymbolTable::String(CUtlSymbol) const` | identical |  |
| utlsymbol | `CUtlRBTree<CUtlSymbolTable::CStringPoolIndex, unsigned short, CUtlSymbolTable::CLess, CUtlMemory<UtlRBTreeNode_t<CUtlSymbolTable::CStringPoolIndex, un` | identical |  |
| vcollide_parse | `ParseKeyvalue(char const*, char (&) [1024], char (&) [1024])` | identical |  |
| vcollide_parse | `CreateVPhysicsKeyParser(char const*)` | identical |  |
| vcollide_parse | `CVPhysicsParse::ParseFluid(fluid_t*, IVPhysicsKeyHandler*)` | identical |  |
| vcollide_parse | `CVPhysicsParse::ParseSolid(solid_t*, IVPhysicsKeyHandler*)` | identical |  |
| vcollide_parse | `CVPhysicsParse::ParseCustom(void*, IVPhysicsKeyHandler*)` | identical |  |
| vcollide_parse | `CVPhysicsParse::ParseVehicle(vehicleparams_t*, IVPhysicsKeyHandler*)` | identical |  |
| vcollide_parse | `CVPhysicsParse::ParseVehicleAxle(vehicle_axleparams_t&)` | identical |  |
| vcollide_parse | `CVPhysicsParse::ParseVehicleBody(vehicle_bodyparams_t&)` | identical |  |
| vcollide_parse | `CVPhysicsParse::ParseSurfaceTable(int*, IVPhysicsKeyHandler*)` | identical |  |
| vcollide_parse | `CVPhysicsParse::ParseVehicleWheel(vehicle_wheelparams_t&)` | identical |  |
| vcollide_parse | `CVPhysicsParse::ParseVehicleEngine(vehicle_engineparams_t&)` | identical |  |
| vcollide_parse | `CVPhysicsParse::ParseVehicleSteering(vehicle_steeringparams_t&)` | identical |  |
| vcollide_parse | `CVPhysicsParse::ParseRagdollConstraint(constraint_ragdollparams_t*, IVPhysicsKeyHandler*)` | identical |  |
| vcollide_parse | `CVPhysicsParse::ParseVehicleSuspension(vehicle_suspensionparams_t&)` | identical |  |
| vcollide_parse | `CVPhysicsParse::ParseVehicleEngineBoost(vehicle_engineparams_t&)` | identical |  |
| vcollide_parse | `CVPhysicsParse::NextBlock()` | identical |  |
| vcollide_parse | `CVPhysicsParse::~CVPhysicsParse()` | identical |  |
| vcollide_parse | `DestroyVPhysicsKeyParser(IVPhysicsKeyParser*)` | identical |  |
| vcollide_parse | `CVPhysicsParse::GetCurrentBlockName()` | identical |  |
| vcollide_parse | `CVPhysicsParse::Finished()` | identical |  |
| vcollide_parse | `CVPhysicsParse::SkipBlock()` | identical |  |
| vcollide_parse | `CVPhysicsParse::~CVPhysicsParse()` | identical |  |
| vector3 | `hk_Vector3::set_rotated_dir(hk_Rotation const&, hk_Vector3 const&)` | identical |  |
| vector3 | `hk_Vector3::set_rotated_inv_dir(hk_Rotation const&, hk_Vector3 const&)` | identical |  |
| vector3 | `hk_Vector3::set_transformed_pos(hk_Transform const&, hk_Vector3 const&)` | identical |  |
| vector3_util | `hk_Vector3_Util::perp_vec(hk_Vector3 const&)` | identical |  |
| vphysics_saverestore | `_GLOBAL__I_a` | identical |  |
| vphysics_saverestore | `CUtlMemory<UtlRBTreeNode_t<CUtlMap<void*, void*, unsigned short>::Node_t, unsigned short>, unsigned short>::Grow(int)` | identical |  |
| vphysics_saverestore | `CUtlRBTree<CUtlMap<void*, void*, unsigned short>::Node_t, unsigned short, CUtlMap<void*, void*, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<` | identical |  |
| vphysics_saverestore | `CUtlRBTree<CUtlMap<void*, void*, unsigned short>::Node_t, unsigned short, CUtlMap<void*, void*, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<` | identical |  |
| vphysics_saverestore | `CUtlRBTree<CUtlMap<void*, void*, unsigned short>::Node_t, unsigned short, CUtlMap<void*, void*, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<` | identical |  |
| vphysics_saverestore | `CUtlRBTree<CUtlMap<void*, void*, unsigned short>::Node_t, unsigned short, CUtlMap<void*, void*, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<` | identical |  |
| vphysics_saverestore | `CUtlRBTree<CUtlMap<void*, void*, unsigned short>::Node_t, unsigned short, CUtlMap<void*, void*, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<` | identical |  |
| vphysics_saverestore | `CUtlRBTree<CUtlMap<void*, void*, unsigned short>::Node_t, unsigned short, CUtlMap<void*, void*, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<` | identical |  |
| vphysics_saverestore | `CPhysicsEnvironment::PreRestore(physprerestoreparams_t const&)` | identical |  |
| vphysics_saverestore | `CPhysicsEnvironment::PostRestore()` | identical |  |
| vphysics_saverestore | `CPhysicsEnvironment::Save(physsaveparams_t const&)` | identical |  |
| vphysics_saverestore | `CPhysicsEnvironment::Restore(physrestoreparams_t const&)` | identical |  |
| vphysics_saverestore | `CVPhysPtrSaveRestoreOps::Restore(SaveRestoreFieldInfo_t const&, IRestore*)` | identical |  |
| vphysics_saverestore | `CVPhysPtrUtlVectorSaveRestoreOps::Restore(SaveRestoreFieldInfo_t const&, IRestore*)` | identical |  |
| vphysics_saverestore | `CUtlMap<void*, void*, unsigned short>::~CUtlMap()` | identical |  |
| vphysics_saverestore | `CUtlRBTree<CUtlMap<void*, void*, unsigned short>::Node_t, unsigned short, CUtlMap<void*, void*, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<` | identical |  |
| vphysics_saverestore | `CUtlRBTree<CUtlMap<void*, void*, unsigned short>::Node_t, unsigned short, CUtlMap<void*, void*, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<` | identical |  |
| vphysics_saverestore | `CUtlRBTree<CUtlMap<void*, void*, unsigned short>::Node_t, unsigned short, CUtlMap<void*, void*, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<` | identical |  |
| vphysics_saverestore | `NoPhysSaveFunc(physsaveparams_t const&, void*)` | identical |  |
| vphysics_saverestore | `NoPhysRestoreFunc(physrestoreparams_t const&, void**)` | identical |  |
| vphysics_saverestore | `CUtlRBTree<CUtlMap<void*, void*, unsigned short>::Node_t, unsigned short, CUtlMap<void*, void*, unsigned short>::CKeyLess, CUtlMemory<UtlRBTreeNode_t<` | identical |  |
| vphysics_saverestore | `CDefSaveRestoreOps::IsEmpty(SaveRestoreFieldInfo_t const&)` | identical |  |
| vphysics_saverestore | `CDefSaveRestoreOps::MakeEmpty(SaveRestoreFieldInfo_t const&)` | identical |  |
| vphysics_saverestore | `CVPhysPtrSaveRestoreOps::Save(SaveRestoreFieldInfo_t const&, ISave*)` | identical |  |
| vphysics_saverestore | `CVPhysPtrUtlVectorSaveRestoreOps::Save(SaveRestoreFieldInfo_t const&, ISave*)` | identical |  |
| vphysics_saverestore | `CDefOps<void*>::LessFunc(void* const&, void* const&)` | identical |  |
