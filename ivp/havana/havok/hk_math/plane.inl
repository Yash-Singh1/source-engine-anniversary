
#ifdef BYTEMATCH_RETAIL
hk_Plane::hk_Plane()
{
}

#endif
hk_Plane::hk_Plane(const hk_Vector3& n, hk_real d)
	: m_plane(n.x,n.y,n.z,d)
{	
}

