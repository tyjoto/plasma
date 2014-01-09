#include "../ParticleListCPU.h"
#include "../FieldDataCPU.h"
#include "../CurrentTallyCPU.h"
#include "../PExitCheck.h"
#include "../ParticleObjNT.inl"

#define DEF_NSPATIAL 3
#define DEF_NVEL 3
#define DEF_IEM false

template<> __attribute__ ((noinline))
void shrink_pushT<1,DEF_NSPATIAL,DEF_NVEL,DEF_IEM,
				ParticleListCPU,FieldDataCPU,CurrentTallyCPU,
				PExitCheckCPU,PExitCheckCPU>(PlasmaData* 		pdata,
				 FieldDataCPU* 		fields,
				 CurrentTallyCPU* 		current,
				 ParticleListCPU* 	plist,
				 int**				iter_array,
				 int**				iptcl,
				 int**				iptcl_new,
				 int&				nptcls_left,
				 int&				nptcl_done,
				 int&				nptcls_process,
				 long long int&				nSubSteps_done)
{
	if(nptcls_left == 1)
	{
		PushNT<1,DEF_NSPATIAL,DEF_NVEL,DEF_IEM,ParticleListCPU,FieldDataCPU,CurrentTallyCPU,
		PExitCheckCPU,PExitCheckCPU>(pdata,fields,current,plist,
							iter_array,iptcl,iptcl_new,
							nptcls_left,nptcl_done,nptcls_process,nSubSteps_done);

	} /* if(1 == nptcls_left) */

}

template<> __attribute__ ((noinline))
void shrink_pushT<VEC_LENGTH_MAX,DEF_NSPATIAL,DEF_NVEL,DEF_IEM,
ParticleListCPU,FieldDataCPU,CurrentTallyCPU,
PExitCheckCPU,PExitCheckCPU>(PlasmaData* 		pdata,
				 FieldDataCPU* 		fields,
				 CurrentTallyCPU* 		current,
				 ParticleListCPU* 	plist,
				 int**				iter_array,
				 int**				iptcl,
				 int**				iptcl_new,
				 int&				nptcls_left,
				 int&				nptcl_done,
				 int&				nptcls_process,
				 long long int&				nSubSteps_done)
{

	if(nptcls_left == VEC_LENGTH_MAX)
	{
		PushNT<VEC_LENGTH_MAX,DEF_NSPATIAL,DEF_NVEL,DEF_IEM,ParticleListCPU,FieldDataCPU,CurrentTallyCPU,
		PExitCheckCPU,PExitCheckCPU>(pdata,fields,current,plist,
							iter_array,iptcl,iptcl_new,
							nptcls_left,nptcl_done,nptcls_process,nSubSteps_done);
	} /* if(ileft == nptcls_left) */
	else
	{
		shrink_pushT<VEC_LENGTH_MAX-1,DEF_NSPATIAL,DEF_NVEL,DEF_IEM,ParticleListCPU,FieldDataCPU,CurrentTallyCPU,
		PExitCheckCPU,PExitCheckCPU>(pdata,fields,current,plist,
							iter_array,iptcl,iptcl_new,
							nptcls_left,nptcl_done,nptcls_process,nSubSteps_done);
	}


}

#undef DEF_IEM
#define DEF_IEM true

template<> __attribute__ ((noinline))
void shrink_pushT<1,DEF_NSPATIAL,DEF_NVEL,DEF_IEM,
				ParticleListCPU,FieldDataCPU,CurrentTallyCPU,
				PExitCheckCPU,PExitCheckCPU>(PlasmaData* 		pdata,
				 FieldDataCPU* 		fields,
				 CurrentTallyCPU* 		current,
				 ParticleListCPU* 	plist,
				 int**				iter_array,
				 int**				iptcl,
				 int**				iptcl_new,
				 int&				nptcls_left,
				 int&				nptcl_done,
				 int&				nptcls_process,
				 long long int&				nSubSteps_done)
{
	if(nptcls_left == 1)
	{
		PushNT<1,DEF_NSPATIAL,DEF_NVEL,DEF_IEM,ParticleListCPU,FieldDataCPU,CurrentTallyCPU,
		PExitCheckCPU,PExitCheckCPU>(pdata,fields,current,plist,
							iter_array,iptcl,iptcl_new,
							nptcls_left,nptcl_done,nptcls_process,nSubSteps_done);

	} /* if(1 == nptcls_left) */

}

template<> __attribute__ ((noinline))
void shrink_pushT<VEC_LENGTH_MAX,DEF_NSPATIAL,DEF_NVEL,DEF_IEM,
ParticleListCPU,FieldDataCPU,CurrentTallyCPU,
PExitCheckCPU,PExitCheckCPU>(PlasmaData* 		pdata,
				 FieldDataCPU* 		fields,
				 CurrentTallyCPU* 		current,
				 ParticleListCPU* 	plist,
				 int**				iter_array,
				 int**				iptcl,
				 int**				iptcl_new,
				 int&				nptcls_left,
				 int&				nptcl_done,
				 int&				nptcls_process,
				 long long int&				nSubSteps_done)
{

	if(nptcls_left == VEC_LENGTH_MAX)
	{
		PushNT<VEC_LENGTH_MAX,DEF_NSPATIAL,DEF_NVEL,DEF_IEM,ParticleListCPU,FieldDataCPU,CurrentTallyCPU,
		PExitCheckCPU,PExitCheckCPU>(pdata,fields,current,plist,
							iter_array,iptcl,iptcl_new,
							nptcls_left,nptcl_done,nptcls_process,nSubSteps_done);
	} /* if(ileft == nptcls_left) */
	else
	{
		shrink_pushT<VEC_LENGTH_MAX-1,DEF_NSPATIAL,DEF_NVEL,DEF_IEM,ParticleListCPU,FieldDataCPU,CurrentTallyCPU,
		PExitCheckCPU,PExitCheckCPU>(pdata,fields,current,plist,
							iter_array,iptcl,iptcl_new,
							nptcls_left,nptcl_done,nptcls_process,nSubSteps_done);
	}


}

