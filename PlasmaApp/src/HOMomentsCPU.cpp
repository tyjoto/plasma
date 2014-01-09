/*-------------------------------------------------------------------------*/
/**
  @file		HOMoments.cu
*/
/*--------------------------------------------------------------------------*/
#include "HOMomentsCPU.h"

#include "ParallelInfo.h"
#include "RunData.h"
#include "math.h"
#include <omp.h>

HOMomentsCPU::HOMomentsCPU(PlasmaData* pdata_in)
{


	pdata = pdata_in;

	device_type = 0;

	int nx = pdata->nx;
	int ny = pdata->ny;
	int nz = pdata->nz;
	nspecies = pdata->nspecies;

	int ntotal = nx*ny*nz*pdata->nspecies;
	long long int alloc_size = nx*ny*nz*nspecies*10;


		all_data = (realkind*)malloc(alloc_size*sizeof(realkind));
//		charge 		= (realkind*)malloc(ntotal*sizeof(realkind));
//
//		currentx 	= (realkind*)malloc(ntotal*sizeof(realkind));
//		currenty 	= (realkind*)malloc(ntotal*sizeof(realkind));
//		currentz 	= (realkind*)malloc(ntotal*sizeof(realkind));
//
//		S2xx = (realkind*)malloc(ntotal*sizeof(realkind));
//		S2xy = (realkind*)malloc(ntotal*sizeof(realkind));
//		S2xz = (realkind*)malloc(ntotal*sizeof(realkind));
//		S2yy = (realkind*)malloc(ntotal*sizeof(realkind));
//		S2yz = (realkind*)malloc(ntotal*sizeof(realkind));
//		S2zz = (realkind*)malloc(ntotal*sizeof(realkind));

	charge = all_data;
	currentx = charge+ntotal;
	currenty = currentx+ntotal;
	currentz = currenty+ntotal;
	S2xx = currentz+ntotal;
	S2xy = S2xx+ntotal;
	S2xz = S2xy+ntotal;
	S2yy = S2xz+ntotal;
	S2yz = S2yy+ntotal;
	S2zz = S2yz+ntotal;

	set_vals(0);
}

void HOMomentsCPU::set_vals(realkind val_in)
{
	int nx = pdata->nx;
	int ny = pdata->ny;
	int nz = pdata->nz;

	int ntotal = nx*ny*nz*pdata->nspecies;

	if(device_type == 0)
	{

		memset(charge,0,ntotal*sizeof(realkind));

		memset(currentx,0,ntotal*sizeof(realkind));
		memset(currenty,0,ntotal*sizeof(realkind));
		memset(currentz,0,ntotal*sizeof(realkind));

		memset(S2xx,0,ntotal*sizeof(realkind));
		memset(S2xy,0,ntotal*sizeof(realkind));
		memset(S2xz,0,ntotal*sizeof(realkind));
		memset(S2yy,0,ntotal*sizeof(realkind));
		memset(S2yz,0,ntotal*sizeof(realkind));
		memset(S2zz,0,ntotal*sizeof(realkind));

//		for(int l=0;l<pdata->nspecies;l++)
//		{
//
//			for(int k=0;k<pdata->nz;k++)
//			{
//
//				for(int j=0;j<pdata->ny;j++)
//				{
//					for(int i=0;i<pdata->nx;i++)
//					{
//						int id_out = i+pdata->nx*(j+pdata->ny*(k+pdata->nz*l));
//
//						charge[id_out] = val_in;
//
//						currentx[id_out] = val_in;
//						currenty[id_out] = val_in;
//						currentz[id_out] = val_in;
//
//						S2xx[id_out] = val_in;
//						S2xy[id_out] = val_in;
//						S2xz[id_out] = val_in;
//						S2yy[id_out] = val_in;
//						S2yz[id_out] = val_in;
//						S2zz[id_out] = val_in;
//					}
//				}
//			}
//		}
	}
	else if(device_type == 1)
	{

#ifndef NO_CUDA
		CUDA_SAFE_CALL(cudaMemset(charge,val_in,ntotal*sizeof(realkind)));
		CUDA_SAFE_CALL(cudaMemset(currentx,val_in,ntotal*sizeof(realkind)));
		CUDA_SAFE_CALL(cudaMemset(currenty,val_in,ntotal*sizeof(realkind)));
		CUDA_SAFE_CALL(cudaMemset(currentz,val_in,ntotal*sizeof(realkind)));
		CUDA_SAFE_CALL(cudaMemset(S2xx,val_in,ntotal*sizeof(realkind)));
		CUDA_SAFE_CALL(cudaMemset(S2xy,val_in,ntotal*sizeof(realkind)));
		CUDA_SAFE_CALL(cudaMemset(S2xz,val_in,ntotal*sizeof(realkind)));
		CUDA_SAFE_CALL(cudaMemset(S2yy,val_in,ntotal*sizeof(realkind)));
		CUDA_SAFE_CALL(cudaMemset(S2yz,val_in,ntotal*sizeof(realkind)));
		CUDA_SAFE_CALL(cudaMemset(S2zz,val_in,ntotal*sizeof(realkind)));

#endif

	}
}

void HOMomentsCPU::apply_weights(void)
{

	omp_set_num_threads(pdata->node_info->nCPU);

	int nx = pdata->nx;
	int ny = pdata->ny;
	int nz = pdata->nz;

	int ntotal = nx*ny*nz*pdata->nspecies;

	realkind dv = pdata->didx*pdata->didy*pdata->didz;

	if(pdata->ndimensions == 1)
		dv = pdata->didx;
	else if(pdata->ndimensions == 2)
		dv = pdata->didx*pdata->didy;

	for(int l=0;l<pdata->nspecies;l++)
	{
		for(int k=0;k<pdata->nz;k++)
		{
//#pragma omp for
			for(int j=0;j<pdata->ny;j++)
			{
//#pragma omp parallel for
				for(int i=0;i<pdata->nx;i++)
				{
					int id_out =  i+pdata->nx*(j+pdata->ny*(k+pdata->nz*l));

					charge[id_out] *= pdata->wspecies[l]*dv;

					currentx[id_out] *= pdata->wspecies[l]*dv;
					currenty[id_out] *= pdata->wspecies[l]*dv;
					currentz[id_out] *= pdata->wspecies[l]*dv;

					S2xx[id_out] *= pdata->wspecies[l]*dv;
					S2xy[id_out] *= pdata->wspecies[l]*dv;
					S2xz[id_out] *= pdata->wspecies[l]*dv;
					S2yy[id_out] *= pdata->wspecies[l]*dv;
					S2yz[id_out] *= pdata->wspecies[l]*dv;
					S2zz[id_out] *= pdata->wspecies[l]*dv;
				}
			}
		}
	}

	if((pdata->node_info->rank_g == 0)||(pdata->rdata->lo_all))
	{

		if(pdata->ndimensions == 1)
		{
			for(int l=0;l<pdata->nspecies;l++)
			{
				for(int k=0;k<pdata->nz;k++)
				{
					for(int j=0;j<pdata->ny;j++)
					{
#pragma omp parallel for
						for(int i=0;i<pdata->nx;i++)
						{
							int id_out =  i+pdata->nx*(j+pdata->ny*(k+pdata->nz*l));
							int id_in =  i+pdata->nx*(0+pdata->ny*(0+pdata->nz*l));

							charge[id_out] = charge[id_in];

							currentx[id_out] = currentx[id_in];
							currenty[id_out] = currenty[id_in];
							currentz[id_out] = currentz[id_in];

							S2xx[id_out] = S2xx[id_in];
							S2xy[id_out] = S2xy[id_in];
							S2xz[id_out] = S2xz[id_in];
							S2yy[id_out] = S2yy[id_in];
							S2yz[id_out] = S2yz[id_in];
							S2zz[id_out] = S2zz[id_in];
						}
					}
				}
			}
		}

		if(pdata->ndimensions == 2)
		{
			for(int l=0;l<pdata->nspecies;l++)
			{
				for(int k=0;k<pdata->nz;k++)
				{
					for(int j=0;j<pdata->ny;j++)
					{
//#pragma omp parallel for
						for(int i=0;i<pdata->nx;i++)
						{
							int id_out =  i+pdata->nx*(j+pdata->ny*(k+pdata->nz*l));
							int id_in =  i+pdata->nx*(j+pdata->ny*(0+pdata->nz*l));

							charge[id_out] = charge[id_in];

							currentx[id_out] = currentx[id_in];
							currenty[id_out] = currenty[id_in];
							currentz[id_out] = currentz[id_in];

							S2xx[id_out] = S2xx[id_in];
							S2xy[id_out] = S2xy[id_in];
							S2xz[id_out] = S2xz[id_in];
							S2yy[id_out] = S2yy[id_in];
							S2yz[id_out] = S2yz[id_in];
							S2zz[id_out] = S2zz[id_in];
						}
					}
				}
			}
		}
	}

	realkind* temp = (realkind*)malloc(ntotal*sizeof(realkind));

	if(pdata->ndimensions == 1)
		for(int l=0;l<pdata->nspecies;l++)
		{
			for(int k=0;k<pdata->nz;k++)
			{
				for(int j=0;j<pdata->ny;j++)
				{
//#pragma omp parallel for
					for(int m=0;m<10;m++)
					{
						enum HOMoments_moment moment;
						if(m==0)
							moment = HOMoments_currentx;
						else if(m==1)
							moment = HOMoments_currenty;
						else if(m==2)
							moment = HOMoments_currentz;
						else if(m==3)
							moment = HOMoments_charge;
						else if(m==4)
							moment = HOMoments_S2xx;
						else if(m==5)
							moment = HOMoments_S2xy;
						else if(m==6)
							moment = HOMoments_S2xz;
						else if(m==7)
							moment = HOMoments_S2yy;
						else if(m==8)
							moment = HOMoments_S2yz;
						else if(m==9)
							moment = HOMoments_S2zz;

#pragma omp parallel for
						for(int i=0;i<pdata->nx;i++)
						{
							temp[i] = 0.25*(get_val(i+1,j,k,l,moment)
									+ 2.0*get_val(i,j,k,l,moment)
									+ get_val(i-1,j,k,l,moment));
						}

#pragma omp parallel for
						for(int i=0;i<pdata->nx;i++)
						{
							get_val(i,j,k,l,moment) = temp[i];
						}
					}
				}
			}
		}
//	else if(pdata->ndimensions == 2)
//		for(int l=0;l<pdata->nspecies;l++)
//		{
//			for(int k=0;k<pdata->nz;k++)
//			{
//
////#pragma omp parallel for
//					for(int m=0;m<10;m++)
//					{
//						enum HOMoments_moment moment;
//						if(m==0)
//							moment = HOMoments_currentx;
//						else if(m==1)
//							moment = HOMoments_currenty;
//						else if(m==2)
//							moment = HOMoments_currentz;
//						else if(m==3)
//							moment = HOMoments_charge;
//						else if(m==4)
//							moment = HOMoments_S2xx;
//						else if(m==5)
//							moment = HOMoments_S2xy;
//						else if(m==6)
//							moment = HOMoments_S2xz;
//						else if(m==7)
//							moment = HOMoments_S2yy;
//						else if(m==8)
//							moment = HOMoments_S2yz;
//						else if(m==9)
//							moment = HOMoments_S2zz;
//
//#pragma omp parallel for
//						for(int j=0;j<pdata->ny;j++)
//						{
//						for(int i=0;i<pdata->nx;i++)
//						{
//							temp[i+pdata->nx*j] = 0.25*(
//									(0.25*(get_val(i+1,j+1,k,l,moment)
//									+ 2.0*get_val(i,j+1,k,l,moment)
//									+ get_val(i-1,j+1,k,l,moment))) +
//
//									2.0*(0.25*(get_val(i+1,j,k,l,moment)
//									+ 2.0*get_val(i,j,k,l,moment)
//									+ get_val(i-1,j,k,l,moment))) +
//
//									(0.25*(get_val(i+1,j-1,k,l,moment)
//									+ 2.0*get_val(i,j-1,k,l,moment)
//									+ get_val(i-1,j-1,k,l,moment)))
//									);
//						}
//						}
//
//#pragma omp parallel for
//						for(int j=0;j<pdata->ny;j++)
//						{
//						for(int i=0;i<pdata->nx;i++)
//						{
//							get_val(i,j,k,l,moment) = temp[i+pdata->nx*j];
//						}
//					}
//				}
//			}
//		}
//
	free(temp);


}

realkind HOMomentsCPU::evaluate_energy(void)
{
	double energy = 0.0;

	if(pdata->ndimensions == 1)
	{
		for(int l=0;l<pdata->nspecies;l++)
			for(int i=0;i<pdata->nx;i++)
			{

				energy += fabs(get_val(i,0,0,l,HOMoments_S2xx)*pdata->mspecies[l]);

				if(pdata->nVelocity > 1)
				energy += (get_val(i,0,0,l,HOMoments_S2yy)+get_val(i,0,0,l,HOMoments_S2zz))*pdata->mspecies[l];
			}

		energy = 0.5*energy*pdata->dxdi/pdata->Lx;
	}
	else if(pdata->ndimensions == 2)
	{
		for(int l=0;l<pdata->nspecies;l++)
		{

			for(int j=0;j<=pdata->ny;j++)
			{
				for(int i=0;i<=pdata->nx;i++)
				{

					energy += get_val(i,j,0,l,HOMoments_S2xx)*pdata->mspecies[l];

					energy += (get_val(i,j,0,l,HOMoments_S2yy)+get_val(i,j,0,l,HOMoments_S2zz))*pdata->mspecies[l];
				}
			}

		}

		energy = energy*pdata->dxdi*pdata->dydi/(pdata->Lx*pdata->Ly);
	}


	return energy;
}

realkind HOMomentsCPU::evaluate_energy(int iS)
{
	double energy = 0.0;

	if(pdata->ndimensions == 1)
	{

		for(int i=0;i<pdata->nx;i++)
		{

			energy += fabs(get_val(i,0,0,iS,HOMoments_S2xx)*pdata->mspecies[iS]);

			if(pdata->nVelocity > 1)
			energy += 0.0*(get_val(i,0,0,iS,HOMoments_S2yy)+get_val(i,0,0,iS,HOMoments_S2zz))*pdata->mspecies[iS];
		}

		energy = 0.5*energy*pdata->dxdi;
	}
	else if(pdata->ndimensions == 2)
	{


			for(int j=0;j<=pdata->ny;j++)
			{
				for(int i=0;i<=pdata->nx;i++)
				{

					energy += get_val(i,j,0,iS,HOMoments_S2xx)*pdata->mspecies[iS];

					if(pdata->nVelocity > 1)
					energy += (get_val(i,j,0,iS,HOMoments_S2yy)+get_val(i,j,0,iS,HOMoments_S2zz))*pdata->mspecies[iS];
				}
			}



		energy = energy*pdata->dxdi*pdata->dydi/(pdata->Lx*pdata->Ly);
	}


	return energy;
}

//__host__
//void HOMomentsCPU::reduce(int tid)
//{
//	tid = omp_get_thread_num();
////	printf("Reducing HO Moments\n");
//	int nthreads = omp_get_num_threads();
//	HOMoments* root_moment = this-tid;
//
//	int ntotal = pdata->nx * pdata->ny * pdata->nz * pdata->nspecies;
//
//	int stride = (ntotal + nthreads - 1)/nthreads;
//
//	int istart = tid * stride;
//	int iend = std::min((tid + 1) * stride-1,ntotal-1);
//
//	//int ndo = iend - istart + 1;
//#pragma omp barrier
//
//	for(int j=1;j<nthreads;j++)
//	{
//		for(int i=istart;i<=iend;i++)
//		{
//			root_moment->charge[i] += (root_moment+j)->charge[i];
//
//			root_moment->currentx[i] += (root_moment+j)->currentx[i];
//			root_moment->currenty[i] += (root_moment+j)->currenty[i];
//			root_moment->currentz[i] += (root_moment+j)->currentz[i];
//
//			root_moment->S2xx[i] += (root_moment+j)->S2xx[i];
//			root_moment->S2xy[i] += (root_moment+j)->S2xy[i];
//			root_moment->S2xz[i] += (root_moment+j)->S2xz[i];
//			root_moment->S2yy[i] += (root_moment+j)->S2yy[i];
//			root_moment->S2yz[i] += (root_moment+j)->S2yz[i];
//			root_moment->S2zz[i] += (root_moment+j)->S2zz[i];
//		}
//
//	}
//
//#pragma omp barrier
//
//	//printf("Finished Reducing HO Moments\n");
//
//}
//
//void HOMomentsCPU::mpi_reduce(HOMoments** all_moments,ParallelInfo* pll_info)
//{
//	//printf("MPI Reducing HO Moments\n");
//	int nthreads = pll_info->nthreads;
//	int tid = pll_info->tid;
//	HOMoments* root_moment = all_moments[1];
//	HOMoments* mpi_root_moment = all_moments[0];
//
//	int ntotal = pdata->nx * pdata->ny * pdata->nz * pdata-> nspecies;
//
//	int stride = (ntotal + nthreads - 1)/nthreads;
//
//	int istart = tid * stride;
//	int iend = std::min((tid + 1) * stride-1,ntotal-1);
//
//	//int ndo = iend - istart + 1;
///*	for(int i=istart;i<iend;i++)
//	{
//		for(int j=1;j<nthreads;j++)
//		{
//			root_moment->charge[i] += all_moments[j+1]->charge[i];
//
//			root_moment->currentx[i] += all_moments[j+1]->currentx[i];
//			root_moment->currenty[i] += all_moments[j+1]->currenty[i];
//			root_moment->currentz[i] += all_moments[j+1]->currentz[i];
//
//			root_moment->S2[i] += all_moments[j+1]->S2[i];
//		}
//
//	}
//
//*/
//
//	if(pdata->device_type == 0){
//	int tid2;
//	omp_set_num_threads(pdata->num_cores);
//#pragma omp parallel private(tid,nthreads,stride) default(shared) num_threads(pdata->num_cores)
//	{
//		tid2 = omp_get_thread_num();
//		(root_moment+tid2)->reduce(tid2);
//
//	}
//	}
//
//
////	MPI_Barrier(MPI_COMM_WORLD);
//
//	// Need to do an MPI reduce
//	if(tid == 0)
//	{
//		for(int i=0;i<pdata->nspecies;i++)
//		{
//			if(pdata->ndimensions == 1)
//				ntotal = pdata->nx;
//			if(pdata->ndimensions == 2)
//				ntotal = pdata->nx*pdata->ny;
//			// Reduce Charge
//			MPI_Allreduce(&get_val(0,0,0,i,HOMoments_charge),&mpi_root_moment->get_val(0,0,0,i,HOMoments_charge),ntotal,MPI_REALKIND,MPI_SUM,MPI_COMM_WORLD);
//
//			// Reduce Current
//			MPI_Allreduce(&get_val(0,0,0,i,HOMoments_currentx),&mpi_root_moment->get_val(0,0,0,i,HOMoments_currentx),ntotal,MPI_REALKIND,MPI_SUM,MPI_COMM_WORLD);
//			if(pdata->nVelocity > 1)MPI_Allreduce(&get_val(0,0,0,i,HOMoments_currenty),&mpi_root_moment->get_val(0,0,0,i,HOMoments_currenty),ntotal,MPI_REALKIND,MPI_SUM,MPI_COMM_WORLD);
//			if(pdata->nVelocity > 2)MPI_Allreduce(&get_val(0,0,0,i,HOMoments_currentz),&mpi_root_moment->get_val(0,0,0,i,HOMoments_currentz),ntotal,MPI_REALKIND,MPI_SUM,MPI_COMM_WORLD);
//
//			MPI_Allreduce(&get_val(0,0,0,i,HOMoments_S2xx),&mpi_root_moment->get_val(0,0,0,i,HOMoments_S2xx),ntotal,MPI_REALKIND,MPI_SUM,MPI_COMM_WORLD);
//			if(pdata->nVelocity > 1)MPI_Allreduce(&get_val(0,0,0,i,HOMoments_S2xy),&mpi_root_moment->get_val(0,0,0,i,HOMoments_S2xy),ntotal,MPI_REALKIND,MPI_SUM,MPI_COMM_WORLD);
//			if(pdata->nVelocity > 2)MPI_Allreduce(&get_val(0,0,0,i,HOMoments_S2xz),&mpi_root_moment->get_val(0,0,0,i,HOMoments_S2xz),ntotal,MPI_REALKIND,MPI_SUM,MPI_COMM_WORLD);
//			if(pdata->nVelocity > 1)MPI_Allreduce(&get_val(0,0,0,i,HOMoments_S2yy),&mpi_root_moment->get_val(0,0,0,i,HOMoments_S2yy),ntotal,MPI_REALKIND,MPI_SUM,MPI_COMM_WORLD);
//			if(pdata->nVelocity > 2)MPI_Allreduce(&get_val(0,0,0,i,HOMoments_S2yz),&mpi_root_moment->get_val(0,0,0,i,HOMoments_S2yz),ntotal,MPI_REALKIND,MPI_SUM,MPI_COMM_WORLD);
//			if(pdata->nVelocity > 2)MPI_Allreduce(&get_val(0,0,0,i,HOMoments_S2zz),&mpi_root_moment->get_val(0,0,0,i,HOMoments_S2zz),ntotal,MPI_REALKIND,MPI_SUM,MPI_COMM_WORLD);
//		}
//	}
//
//	if((pll_info->myid_mpi == 0)||(pdata->lo_all))
//	{
//
//		if(pdata->ndimensions == 1)
//		{
//			for(int l=0;l<pdata->nspecies;l++)
//			{
//				for(int k=0;k<pdata->nz;k++)
//				{
//					for(int j=0;j<pdata->ny;j++)
//					{
//						for(int i=0;i<pdata->nx;i++)
//						{
//							int id_out =  i+pdata->nx*(j+pdata->ny*(k+pdata->nz*l));
//							int id_in =  i+pdata->nx*(0+pdata->ny*(0+pdata->nz*l));
//
//							mpi_root_moment->charge[id_out] = mpi_root_moment->charge[id_in];
//
//							mpi_root_moment->currentx[id_out] = mpi_root_moment->currentx[id_in];
//							mpi_root_moment->currenty[id_out] = mpi_root_moment->currenty[id_in];
//							mpi_root_moment->currentz[id_out] = mpi_root_moment->currentz[id_in];
//
//							mpi_root_moment->S2xx[id_out] = mpi_root_moment->S2xx[id_in];
//							mpi_root_moment->S2xy[id_out] = mpi_root_moment->S2xy[id_in];
//							mpi_root_moment->S2xz[id_out] = mpi_root_moment->S2xz[id_in];
//							mpi_root_moment->S2yy[id_out] = mpi_root_moment->S2yy[id_in];
//							mpi_root_moment->S2yz[id_out] = mpi_root_moment->S2yz[id_in];
//							mpi_root_moment->S2zz[id_out] = mpi_root_moment->S2zz[id_in];
//						}
//					}
//				}
//			}
//		}
//
//		if(pdata->ndimensions == 2)
//		{
//			for(int l=0;l<pdata->nspecies;l++)
//			{
//				for(int k=0;k<pdata->nz;k++)
//				{
//					for(int j=0;j<pdata->ny;j++)
//					{
//						for(int i=0;i<pdata->nx;i++)
//						{
//							int id_out =  i+pdata->nx*(j+pdata->ny*(k+pdata->nz*l));
//							int id_in =  i+pdata->nx*(j+pdata->ny*(0+pdata->nz*l));
//
//							mpi_root_moment->charge[id_out] = mpi_root_moment->charge[id_in];
//
//							mpi_root_moment->currentx[id_out] = mpi_root_moment->currentx[id_in];
//							mpi_root_moment->currenty[id_out] = mpi_root_moment->currenty[id_in];
//							mpi_root_moment->currentz[id_out] = mpi_root_moment->currentz[id_in];
//
//							mpi_root_moment->S2xx[id_out] = mpi_root_moment->S2xx[id_in];
//							mpi_root_moment->S2xy[id_out] = mpi_root_moment->S2xy[id_in];
//							mpi_root_moment->S2xz[id_out] = mpi_root_moment->S2xz[id_in];
//							mpi_root_moment->S2yy[id_out] = mpi_root_moment->S2yy[id_in];
//							mpi_root_moment->S2yz[id_out] = mpi_root_moment->S2yz[id_in];
//							mpi_root_moment->S2zz[id_out] = mpi_root_moment->S2zz[id_in];
//						}
//					}
//				}
//			}
//		}
//	}
//
////	printf("Finished Reducing HO Moments with n_nodes = %i\n",pll_info->n_nodes);
//
//
//}

void HOMomentsCPU::copy_from(HOMomentsCPU* src)
{
	// Copy all the moment values from src to this
	int nalloc = pdata->nx*pdata->ny*pdata->nz*pdata->nspecies;

	if((device_type == 0)&&(src->device_type == 0))
	{
//		memcpy(charge,src->charge,nalloc*sizeof(realkind));
//		memcpy(currentx,src->currentx,nalloc*sizeof(realkind));
//		memcpy(currenty,src->currenty,nalloc*sizeof(realkind));
//		memcpy(currentz,src->currentz,nalloc*sizeof(realkind));
//
//		memcpy(S2xx,src->S2xx,nalloc*sizeof(realkind));
//		memcpy(S2xy,src->S2xy,nalloc*sizeof(realkind));
//		memcpy(S2xz,src->S2xz,nalloc*sizeof(realkind));
//		memcpy(S2yy,src->S2yy,nalloc*sizeof(realkind));
//		memcpy(S2yz,src->S2yz,nalloc*sizeof(realkind));
//		memcpy(S2zz,src->S2zz,nalloc*sizeof(realkind));

		memcpy(all_data,src->all_data,10*nalloc*sizeof(realkind));
	}
	else
	{

#ifndef NO_CUDA
		enum cudaMemcpyKind kind;

		if(device_type == 1)
		{
			if(src->device_type == 1)

				kind = cudaMemcpyDeviceToDevice;
			else if(src->device_type == 0)
				kind = cudaMemcpyHostToDevice;
		}
		else
			kind = cudaMemcpyDeviceToHost;

		CUDA_SAFE_CALL(cudaMemcpy(charge,src->charge,nalloc*sizeof(realkind),kind));

		CUDA_SAFE_CALL(cudaMemcpy(currentx,src->currentx,nalloc*sizeof(realkind),kind));
		CUDA_SAFE_CALL(cudaMemcpy(currenty,src->currenty,nalloc*sizeof(realkind),kind));
		CUDA_SAFE_CALL(cudaMemcpy(currentz,src->currentz,nalloc*sizeof(realkind),kind));

		CUDA_SAFE_CALL(cudaMemcpy(S2xx,src->S2xx,nalloc*sizeof(realkind),kind));
		CUDA_SAFE_CALL(cudaMemcpy(S2xy,src->S2xy,nalloc*sizeof(realkind),kind));
		CUDA_SAFE_CALL(cudaMemcpy(S2xz,src->S2xz,nalloc*sizeof(realkind),kind));
		CUDA_SAFE_CALL(cudaMemcpy(S2yy,src->S2yy,nalloc*sizeof(realkind),kind));
		CUDA_SAFE_CALL(cudaMemcpy(S2yz,src->S2yz,nalloc*sizeof(realkind),kind));
		CUDA_SAFE_CALL(cudaMemcpy(S2zz,src->S2zz,nalloc*sizeof(realkind),kind));
#endif
	}





}

void HOMomentsCPU::copy_to(HOMomentsCPU* dst)
{
	dst->copy_from(this);
}

//realkind HOMomentsCPU::check_charge(HOMomentsCPU* moments_old)
//{
//	double charge_cons = 0;
//
//	int j=0;
//	int k=0;
//
//	realkind dv = pdata->didx*pdata->didy*pdata->didz;
//if(pdata->ndimensions == 1)
//	for(int i=0;i<pdata->nx;i++)
//	{
//		realkind deltaj  = 0;
//		realkind deltan = 0;
//
//		for(int l=0;l<pdata->nspecies;l++)
//		{
//			deltaj += pdata->qspecies[l]*(get_val(i+1,j,k,l,HOMoments_currentx)-get_val(i,j,k,l,HOMoments_currentx))*pdata->didx;
//			deltan += pdata->qspecies[l]*(get_val(i,j,k,l,HOMoments_charge)-moments_old->get_val(i,j,k,l,HOMoments_charge))/(pdata->dt);
//
//		}
//
//	//	printf("charge_cons(%i) = %e + %e = %e\n",i,deltaj,deltan,deltaj+deltan);
//
//		charge_cons += fabs(deltan + deltaj)/(pdata->Lx*pdata->Ly);
//	}
//else{
//	for(int k=0;k<pdata->nz-1;k++)
//	{
//		for(int j=0;j<pdata->ny;j++)
//		{
//			for(int i=0;i<pdata->nx;i++)
//			{
//				realkind deltaj  = 0;
//				realkind deltan = 0;
//
//				for(int l=0;l<pdata->nspecies;l++)
//				{
//					deltaj += (get_val(i+1,j,k,l,HOMoments_currentx)-get_val(i,j,k,l,HOMoments_currentx))*pdata->didx;
//					deltan += (get_val(i,j,k,l,HOMoments_charge)-moments_old->get_val(i,j,k,l,HOMoments_charge))/(pdata->dt);
//
//					if(pdata->ndimensions >1)
//					deltaj += (get_val(i,j+1,k,l,HOMoments_currenty)-get_val(i,j,k,l,HOMoments_currenty))*pdata->didx;
//
//				}
//
//			//	printf("charge_cons(%i) = %e + %e = %e\n",i,deltaj,deltan,deltaj+deltan);
//
//				charge_cons += fabs(deltan + deltaj)/(pdata->Lx*pdata->Ly);
//			}
//		}
//	}
//}
//
//
//	return fabs(charge_cons);
//}

/*
void HOMoments::reduce(HOMoments** all_moments,ParallelInfo* pll_info)
{
	//printf("Reducing HO Moments\n");
	int nthreads = pll_info->nthreads;
	int tid = pll_info->tid;
	HOMoments* root_moment = all_moments[1];
	HOMoments* mpi_root_moment = all_moments[0];

	int ntotal = pdata->nx * pdata->ny * pdata->nz;

	int stride = (ntotal + nthreads - 1)/nthreads;

	int istart = tid * stride;
	int iend = fmin((tid + 1) * stride-1,ntotal-1);

	//int ndo = iend - istart + 1;
#pragma omp barrier

	for(int i=istart;i<iend;i++)
	{
		for(int j=1;j<nthreads;j++)
		{
			root_moment->charge[i] += all_moments[j+1]->charge[i];

			root_moment->currentx[i] += all_moments[j+1]->currentx[i];
			root_moment->currenty[i] += all_moments[j+1]->currenty[i];
			root_moment->currentz[i] += all_moments[j+1]->currentz[i];

			root_moment->S2[i] += all_moments[j+1]->S2[i];
		}

	}

#pragma omp barrier

	printf("Finished Reducing HO Moments with n_nodes = %i\n",pll_info->n_nodes);

	if(pll_info->n_nodes > 1)
	{
		// Need to do an MPI reduce
		if(tid == 0)
		{
			// Reduce Charge
			MPI_Allreduce(charge,mpi_root_moment->charge,ntotal,MPI_FLOAT,MPI_SUM,MPI_COMM_WORLD);

			// Reduce Current
			MPI_Allreduce(currentx,mpi_root_moment->currentx,ntotal,MPI_FLOAT,MPI_SUM,MPI_COMM_WORLD);
			MPI_Allreduce(currenty,mpi_root_moment->currenty,ntotal,MPI_FLOAT,MPI_SUM,MPI_COMM_WORLD);
			MPI_Allreduce(currentz,mpi_root_moment->currentz,ntotal,MPI_FLOAT,MPI_SUM,MPI_COMM_WORLD);
		}
	}
	else
	{
		for(int i=istart;i<iend;i++)
		{

			mpi_root_moment->charge[i] = root_moment->charge[i];

			mpi_root_moment->currentx[i] = root_moment->currentx[i];
			mpi_root_moment->currenty[i] = root_moment->currenty[i];
			mpi_root_moment->currentz[i] = root_moment->currentz[i];

			mpi_root_moment->S2[i] = root_moment->S2[i];

		}
	}
#pragma omp barrier

}

*/
__attribute__((noinline))
realkind& HOMomentsCPU::get_val(const int ix, const int iy, const int iz,
		const int ispecies,enum HOMoments_moment moment)
{
	realkind* result;

	int ix2,iy2,iz2;


	ix2 = ((ix%pdata->nx)+pdata->nx)%pdata->nx;
	iy2 = ((iy%pdata->ny)+pdata->ny)%pdata->ny;
	iz2 = ((iz%pdata->nz)+pdata->nz)%pdata->nz;


	int iout = ix2 + pdata->nx * (iy2 + pdata->ny * (iz2 + pdata->nz * ispecies));
	switch(moment)
	{
	case HOMoments_charge:
		result = charge + iout;
		break;
	case HOMoments_currentx:
		result = currentx + iout;
		break;
	case HOMoments_currenty:
		result = currenty + iout;
		break;
	case HOMoments_currentz:
		result = currentz + iout;
		break;
	case HOMoments_S2xx:
		result = S2xx + iout;
		break;
	case HOMoments_S2xy:
		result = S2xy + iout;
		break;
	case HOMoments_S2xz:
		result = S2xz + iout;
		break;
	case HOMoments_S2yy:
		result = S2yy + iout;
		break;
	case HOMoments_S2yz:
		result = S2yz + iout;
		break;
	case HOMoments_S2zz:
		result = S2zz + iout;
		break;
	default:
		break;
	}

	return *result;
}

__attribute__((noinline))
const realkind& HOMomentsCPU::get_val(const int ix, const int iy, const int iz,
		const int ispecies,enum HOMoments_moment moment)
const
{
	realkind* result;

	int ix2,iy2,iz2;

	ix2 = ((ix%pdata->nx)+pdata->nx)%pdata->nx;
	iy2 = ((iy%pdata->ny)+pdata->ny)%pdata->ny;
	iz2 = ((iz%pdata->nz)+pdata->nz)%pdata->nz;


	int iout = ix2 + pdata->nx * (iy2 + pdata->ny * (iz2 + pdata->nz * ispecies));
	switch(moment)
	{
	case HOMoments_charge:
		result = charge + iout;
		break;
	case HOMoments_currentx:
		result = currentx + iout;
		break;
	case HOMoments_currenty:
		result = currenty + iout;
		break;
	case HOMoments_currentz:
		result = currentz + iout;
		break;
	case HOMoments_S2xx:
		result = S2xx + iout;
		break;
	case HOMoments_S2xy:
		result = S2xy + iout;
		break;
	case HOMoments_S2xz:
		result = S2xz + iout;
		break;
	case HOMoments_S2yy:
		result = S2yy + iout;
		break;
	case HOMoments_S2yz:
		result = S2yz + iout;
		break;
	case HOMoments_S2zz:
		result = S2zz + iout;
		break;
	default:
		break;
	}

	return *result;
}
