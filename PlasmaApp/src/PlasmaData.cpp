#include "PlasmaData.h"
#include <mpi.h>
#include <omp.h>
#include <list>
#include <string>
#include <string.h>
#include <cctype>
#include <tr1/random>
#include <random>
#include "Util/mkpath.h"
#include "Util/OutputPage.h"
#include "ParallelInfo.h"
#include "OutputCtrl.h"
#include "RunData.h"
#include "Normalization.h"
#include "Util/webui/LogStream.h"




LogStream LO_log = *(new LogStream("logger1"));
LogStream HO_log = *(new LogStream("logger2"));
LogStream Outer_log = *(new LogStream("logger3"));
LogStream debug_log = *(new LogStream("logger4"));


realkind epsilon_naught;
realkind mu_naught;

const realkind pi_const = 3.14159265359;

std::tr1::uniform_int<int> rand_unif;

std::tr1::ranlux64_base_01 gen1;
std::tr1::mt19937 gen2;
std::tr1::linear_congruential<unsigned long, 33614, 0, 2147483647> seeder;
std::tr1::ranlux64_base_01 generator;


std::tr1::xor_combine<std::tr1::ranlux64_base_01,7,std::tr1::mt19937,13> xorgen;

bool irand = 0;

double drandom()
{
	if(!irand)
	{
		// initialize the seeder
		seeder.seed((rand()%2386157)+(rand()%98463));



		// combined engine
		xorgen = *(new std::tr1::xor_combine<std::tr1::ranlux64_base_01,7,std::tr1::mt19937,13>(gen1, gen2));

		// initialize the combined generator
		xorgen.seed(seeder);



		rand_unif = *(new std::tr1::uniform_int<int>(0,1000000-1));

		irand = 1;
	}


	return (rand_unif(xorgen)+1.0e-6)*1.0e-6;

}

int isPowerOfTwo (unsigned int x)
{
  return ((x != 0) && ((x & (~x + 1)) == x));
}



//PlasmaData::PlasmaData(int argc,char* argv[]) : argc_s(argc),argv_s(argv)
//{
//
//	node_info = new ParallelInfo();
//	rdata = new RunData();
//	rstatus = new RunStatus();
//
////	system("numactl --nodebind=0,1 --physcpubind=0-15");
//	system("numactl --show");
//
//	setvbuf( stdout, NULL, _IONBF, 0 );
//
//	// Default Values
//	nx = 32;
//	ny = 2;
//	nz = 2;
//
//	nspecies = 1;
//	nptcls = 6400;
//
//	Lx = 1.0;
//	Ly = 1.0;
//	Lz = 1.0;
//
//	xmin = 0;
//	ymin = 0;
//	zmin = 0;
//
//	n0 = 1;
//
//	omega_pe = 1.0;
//	Te = 1.0;
//
//	epsilon_r = 1.0e-10;
//	epsilon_a = 1.0e-4;
//	niter_max = 40;
//	nSubcycle_max = 1000;
//	nSubcycle_min = 4;
//
//
//
//	dt = 0.5;
//	nsteps = 20;
//
//	ndimensions = 1;
//	nVelocity = 1;
//	iEM = 0;
//
//
//
//
//	rdata->plot_flag = 0;
//	rdata->lo_all = 0;
//	rdata->runid = 0;
//
//
//
//	nreturn = 1;
//
//	tol_outer_picard = 1.0e-6;
//	nOuterPicard_max = 40;
//
//
//
//
//	rdata->SimName = "unknown";
//
//
//
//
//	// Check for an input file
//	for(int i=0;i<argc;i++)
//	{
//		std::string arg(argv[i]);
//		if(arg == ("-f"))
//		{
//			// Need to read the input file first
//			std::string inFile(argv[i+1]);
//			readInput(inFile);
//			break;
//
//		}
//	}
//
//	thread_info = node_info->init_info(argc,argv);
//
//	for(int i=0;i<node_info->nDevices2;i++)
//		printf("iDevice[%i] = %i, igpu = %i\n",i,thread_info[i]->device_id,thread_info[i]->gpu_info->igpu);
//
//	// Command line arguments overwrite input file
//	for(int i=0;i<argc;i++)
//	{
//		std::string arg(argv[i]);
//
//		if(arg == ("-nx"))
//			nx = atoi(argv[i+1]);
//		else if(arg == ("-ny"))
//			ny = atoi(argv[i+1]);
//		else if(arg == ("-nz"))
//			nz = atoi(argv[i+1]);
//
//		else if(arg == ("-Lx"))
//			Lx = atof(argv[i+1]);
//		else if(arg == ("-Ly"))
//			Ly = atof(argv[i+1]);
//		else if(arg == ("-Lz"))
//			Lz = atof(argv[i+1]);
//
//		else if(arg == ("-x0"))
//			xmin = atof(argv[i+1]);
//		else if(arg == ("-y0"))
//			ymin = atof(argv[i+1]);
//		else if(arg == ("-z0"))
//			zmin = atof(argv[i+1]);
//
//		else if(arg == ("--vec_length"))
//			node_info->cpu_info->vec_length = atoi(argv[i+1]);
//		else if(arg == ("-dt"))
//			dt = atof(argv[i+1]);
//		else if(arg == ("-s"))
//			nsteps = atoi(argv[i+1]);
//
//		else if(arg == ("-np"))
//			nptcls = atoi(argv[i+1]);
//		else if(arg == ("-ns"))
//			nspecies = atoi(argv[i+1]);
//		else if(arg == "--min-subcycles")
//			nSubcycle_min = atoi(argv[i+1]);
//
//		else if(arg == "--epsilon")
//			epsilon_r = atof(argv[i+1]);
//
//		else if(arg == "-n0")
//			n0 = atof(argv[i+1]);
//		else if(arg == "-Te")
//			Te = atof(argv[i+1]);
//		else if(arg == "-We")
//			omega_pe = atof(argv[i+1]);
//
//		else if(arg == "--num_cores")
//			node_info->nCPU = atoi(argv[i+1]);
//
//		else if(arg == "--gpu-mult")
//			node_info->gpu_info->multiplier = atof(argv[i+1]);
//
//		else if(arg == "-g")
//			rdata->plot_flag = 1;
//		else if(arg == "--lo-all")
//			rdata->lo_all = 1;
//		else if(arg == "--runid")
//			rdata->runid = atoi(argv[i+1]);
//
//		else if(arg == "--nSpatial")
//			ndimensions = atoi(argv[i+1]);
//		else if(arg == "--nVel")
//			nVelocity = atoi(argv[i+1]);
//
//	}
//
//
//	// Check to make sure that nx, ny, and nz are powers of 2
//	if(!isPowerOfTwo(nx))
//	{	if(node_info->rank_g == 0)
//		printf("Error!: nx = %i is not a power of 2\n",nx);
//		exit(0);
//	}
//	else if(!isPowerOfTwo(ny))
//	{	if(node_info->rank_g == 0)
//		printf("Error!: ny = %i is not a power of 2\n",ny);
//		exit(0);
//	}
//	else if(!isPowerOfTwo(nz))
//	{
//		if(node_info->rank_g == 0)
//		printf("Error!: nz = %i is not a power of 2\n",nz);
//		exit(0);
//	}
//
//
//	this->setup();
//
//	float mion = 2000.0;
//
//	for(int i=0;i<3;i++)
//	{
//		nptcls_device[i] = nptcls;
//	}
//
//	nptcls_device[1] = nptcls;
//
//
////	mion = 100;
//
//	int nptcls_species_temp[NSPECIES_MAX];
//	for(int i=0;i<NSPECIES_MAX;i++)
//	{
//		nptcls_species_temp[i] = 0;
//	}
//
//	for(int i=0;i<node_info->nDevices2;i++)
//		nptcls_species_temp[thread_info[i]->ispecies] += nptcls_device[thread_info[i]->device_type];
//
//	// Reduce the number of particles for each species
//	MPI_Allreduce(nptcls_species_temp,nptcls_species,
//			NSPECIES_MAX,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
//
//	//printf("num nodes = %i\n",num_nodes);
//
//	for(int i=0;i<NSPECIES_MAX;i++)
//	{
//		qspecies[i] = -1;
//		mspecies[i] = i*mion;
//	}
//
//	qspecies[0] = 1;
//	mspecies[0] = 1;
//
//	for(int i=0;i<NSPECIES_MAX;i++)
//	{
//		wspecies[i] = n0*Lx*Ly*Lz/((double)nptcls_species[i]);
//
//		printf("wspecies[%i] = %e\n",i,wspecies[i]);
//	}
//
//	unsigned long long int FNV_prime = 1099511628211ULL;
//	unsigned long long int FNV_offset_basis = 14695981039346656037ULL;
//	char data_octet[19];
//	unsigned long long int hash = FNV_offset_basis;
//
//	int nptcls_t=0;
//	for(int i=0;i<nspecies;i++)
//		nptcls_t += nptcls_species[i];
//
//	data_octet[0] = nx;
//	data_octet[1] = nptcls_t*1.0e-5;
//	data_octet[2] = ndimensions+3*nVelocity;
//	data_octet[3] = Lx;
//	data_octet[4] = nspecies;
//	data_octet[5] = node_info->nTasks_g;
//	data_octet[6] = node_info->nCPU;
//	data_octet[7] = node_info->cpu_info->vec_length;
//	data_octet[8] = sizeof(realkind);
//	data_octet[9] = dt*10;
//	data_octet[10] = nsteps;
//	data_octet[11] = rdata->lo_all+1;
//	data_octet[12] = device_multiplier[1];
//	data_octet[13] = iEM+1;
//	data_octet[14] = ny;
//	data_octet[15] = nz;
//	data_octet[16] = Ly;
//	data_octet[17] = Lz;
//	data_octet[18] = node_info->nDevices;
//
//	for(int i=0;i<19;i++)
//	{
//		hash *= FNV_prime;
//		hash = hash ^ data_octet[i];
//	}
//
//	sprintf(rdata->output_id,"%X",hash);
//
//	timespec res;
//	clock_getres(CLOCK_REALTIME,&res);
//	tmax = nsteps * dt;
//
//	nreturn = std::min((int)nreturn,node_info->cpu_info->vec_length);
//
//	if(node_info->rank_g == 0){
//
//	// Print out the simulation parameters
//	printf("############################################\n");
//	printf("Simulation will be run with the following parameters:\n");
//	// Number of cells
//	printf("\n/********* Spatial Parameters *********/\n");
//	printf("Grid Dimensions (nx,ny,nz): %i, %i, %i\n",nx,ny,nz);
//	printf("System Lengths (Lx,Ly,Lz): %e, %e, %e\n",Lx,Ly,Lz);
//	printf("System Origins (x0,y0,z0): %e, %e, %e\n",xmin,ymin,zmin);
//
//	printf("\n/********* Particle Parameters *********/\n");
//	printf("Number of particles per species: %i\n",nptcls);
//	printf("Number of species: %i\n",nspecies);
//
//	printf("\n/********* Time Step Parameters *********/\n");
//	printf("Total Simulation Time: %e\n",tmax);
//	printf("Time Step Size: %e\n",dt);
//
//	printf("\n/********* Other Parameters *********/\n");
//	printf("Relative Tolerance: %e\n",epsilon_r);
//	printf("Absolute Tolerance: %e\n",epsilon_a);
//	printf("CPU Vector Length: %i\n",node_info->cpu_info->vec_length);
//	printf("Number of CPU Cores to Use: %i\n",node_info->nCPU);
//	printf("Output ID: %s\n",rdata->output_id);
//	printf("Clock resolution: %e (ns)\n",1.0e9*res.tv_sec+res.tv_nsec);
//	printf("Sizeof or realkind is %i\n",sizeof(realkind));
//	printf("############################################\n");
//
//	}
//
//	output_ctrl = new OutputCtrl(this,argc,argv);
//
//
//
//
//
//}

PlasmaData::PlasmaData(int argc,char* argv[],Normalization* normalizer) : argc_s(argc),argv_s(argv)
{

	node_info = new ParallelInfo();
	rdata = new RunData();
	rstatus = new RunStatus();

//	system("numactl --nodebind=0,1 --physcpubind=0-15");
	system("numactl --show");

	setvbuf( stdout, NULL, _IONBF, 0 );

	parser = new InputParser(argc,argv);

	thread_info = node_info->init_info(argc,argv);



	// Default Values
	parser->GetParam("NUMBER_CELLS_X","-nx",nx,32);
	parser->GetParam("NUMBER_CELLS_Y","-ny",ny,2);
	parser->GetParam("NUMBER_CELLS_Z","-nz",nz,2);

	parser->GetParam("NUMBER_SPECIES","-ns",nspecies,1);
	parser->GetParam("NUMBER_PARTICLES","-np",nptcls,6400);

	realkind mass_ratio = mi_p1/me_p;
	parser->GetParam("MASS_RATIO","-mr",mass_ratio,mass_ratio);

	mi_p = mass_ratio*me_p;


	for(int i=0;i<node_info->nDevices2;i++)
		printf("iDevice[%i] = %i, igpu = %i\n",i,thread_info[i]->device_id,thread_info[i]->gpu_info->igpu);


	// Check to make sure that nx, ny, and nz are powers of 2
	if(!isPowerOfTwo(nx))
	{	if(node_info->rank_g == 0)
		printf("Error!: nx = %i is not a power of 2\n",nx);
		exit(0);
	}
	else if(!isPowerOfTwo(ny))
	{	if(node_info->rank_g == 0)
		printf("Error!: ny = %i is not a power of 2\n",ny);
		exit(0);
	}
	else if(!isPowerOfTwo(nz))
	{
		if(node_info->rank_g == 0)
		printf("Error!: nz = %i is not a power of 2\n",nz);
		exit(0);
	}


	for(int i=0;i<3;i++)
	{
		nptcls_device[i] = nptcls;
	}

	nptcls_device[1] = 2*nptcls;


//	mion = 100;

	int nptcls_species_temp[NSPECIES_MAX];
	for(int i=0;i<NSPECIES_MAX;i++)
	{
		nptcls_species_temp[i] = 0;
	}

	for(int i=0;i<node_info->nDevices2;i++)
	{
		nptcls_species_temp[thread_info[i]->ispecies] += nptcls_device[thread_info[i]->device_type];
		printf("nptcls_species[%i] = %i\n",i,nptcls_device[thread_info[i]->device_type]);

	}

	// Reduce the number of particles for each species
	MPI_Allreduce(nptcls_species_temp,nptcls_species,
			NSPECIES_MAX,MPI_INT,MPI_SUM,MPI_COMM_WORLD);

	for(int i=0;i<NSPECIES_MAX;i++)
	{
		printf("nptcls_species[%i] = %i\n",i,nptcls_species[i]);
	}


	parser->GetParam("PHYSICAL_ELECTRON_DENSITY","-ne_p",ne_p,epsilon_0_p/(qe_p*qe_p));
	parser->GetParam("PHYSICAL_ION_DENSITY","-ni_p",ni_p,epsilon_0_p/(qe_p*qe_p));


	parser->GetParam("MASS_NORMALIZER","-m0",m0,me_p);
	parser->GetParam("CHARGE_NORMALIZER","-q0",q0,qe_p);

	parser->GetParam("DENSITY_NORMALIZER","-n0",n0,ne_p);
	parser->GetParam("PHYSICAL_ELECTRON_TEMPERATURE","-Te_p",Te_p,q0*q0*pow(ne_p,1.0/3.0)/epsilon_0_p);
	parser->GetParam("PHYSICAL_ION_TEMPERATURE","-Ti_p",Ti_p,q0*q0*pow(ni_p,1.0/3.0)/epsilon_0_p);

	parser->GetParam("TEMPERATURE_NORMALIZER","-T0",T0,q0*q0*pow(n0,1.0/3.0)/epsilon_0_p);
	parser->GetParam("LENGTH_NORMALIZER","-L0",L0,sqrt(epsilon_0_p*Te_p/(ne_p*qe_p*qe_p)));

	parser->GetParam("PHYSICAL_SYSTEM_LENGTH_X","-Lx_p",Lx_p,L0);
	parser->GetParam("PHYSICAL_SYSTEM_LENGTH_Y","-Ly_p",Ly_p,L0);
	parser->GetParam("PHYSICAL_SYSTEM_LENGTH_Z","-Lz_p",Lz_p,L0);


	parser->GetParam("NORMALIZED_SYSTEM_LENGTH_X","-Lx",Lx,Lx_p/L0);
	parser->GetParam("NORMALIZED_SYSTEM_LENGTH_Y","-Ly",Ly,Ly_p/L0);
	parser->GetParam("NORMALIZED_SYSTEM_LENGTH_Z","-Lz",Lz,Lz_p/L0);

	parser->GetParam("NORMALIZED_SYSTEM_ORIGIN_X","-x0",xmin,0.0);
	parser->GetParam("NORMALIZED_SYSTEM_ORIGIN_Y","-y0",ymin,0.0);
	parser->GetParam("NORMALIZED_SYSTEM_ORIGIN_Z","-z0",zmin,0.0);

	parser->GetParam("TIME_STEP_SIZE","-dt",dt,0.5);
	parser->GetParam("NUMBER_TIME_STEPS","-s",nsteps,100);
	parser->GetParam("MINIMUM_NUMBER_SUBCYCLES","--min-subcycles",nSubcycle_min,2);
	parser->GetParam("MAXIMUM_INNER_PICARD","--ppicard-max",niter_max,30);


	parser->GetParam("CPU_VECTOR_LENGTH","--vec_length",node_info->cpu_info->vec_length,32);

	parser->GetParam("PARTICLE_PICARD_TOLERANCE","--epsilon",epsilon_r,1.0e-6);
	parser->GetParam("PARTICLE_PICARD_TOLERANCE_A","--epsilon-a",epsilon_a,1.0e-4);


	parser->GetParam("NUMBER_CPU_CORES","--num-cores",node_info->nCPU,32);

	parser->GetParam("GPU_NPTCL_MULTIPLIER","--gpu-mult",node_info->gpu_info->multiplier,1.0f);


	parser->GetParam("PLOT_FLAG","-g",rdata->plot_flag,true);
	parser->GetParam("LO_SOLVE_ALL_NODES","--lo-all",rdata->lo_all,true);
	parser->GetParam("RUNID","--runid",rdata->runid,0);
	parser->GetParam("N_SPATIAL_DIMENSIONS","--nSpatial",ndimensions,1);
	parser->GetParam("N_VELOCITY_DIMENSIONS","--nVel",nVelocity,1);
	parser->GetParam("ELECTROMAGNETIC_FLAG","-iEM",iEM,0);

	parser->GetParam("OUTER_PICARD_TOLERANCE","--epsilon-op",tol_outer_picard,1.0e-6);
	parser->GetParam("MAX_OUTER_PICARD_ITERATIONS","--max-op",nOuterPicard_max,40);

	rdata->SimName = "unknown";
	parser->GetParam("SIMULATION_NAME","--sim-name",rdata->SimName,rdata->SimName);
	pythonScript = new std::string();
	std::string tmp_pythonScript = "Viz/Examples/twostream.py";
	parser->GetParam("PYTHON_SCRIPT","--python",tmp_pythonScript,tmp_pythonScript);

	*pythonScript = tmp_pythonScript;

	normalizer -> CalcNorms(this,parser);


	xmin *= Lx;
	ymin *= Ly;
	zmin *= Lz;

	q_h[0] = qe_p/q0;
	q_h[1] = qi_p/q0;
	m_h[0] = me_p/m0;
	m_h[1] = mi_p/m0;
	n_bh[0] = ne_p/n0;
	n_bh[1] = ni_p/n0;
	T_h[0] = Te_p/T0;
	T_h[1] = Ti_p/T0;

	xi = sqrt(epsilon_0_p*T0/(q0*q0*n0*L0*L0));
	bobzeta = m0/(q0*q0*L0*L0*mu_0_p*n0);

	parser->GetParam("XI_NORMALIZATION","--epsilon-op",xi,1.0e-6);
	parser->GetParam("ZETA_NORMALIZATION","--max-op",bobzeta,40.0);

//	bobzeta = 1.0;

	nSubcycle_max = 1000;

	eps0_h = xi*xi;
	mu0_h = 1.0/bobzeta;

//	xi = 1.0;
//	bobzeta = 1.0;
//	eps0_h = 1.0;
//	mu0_h = 1.0;


	dxdi = Lx/((realkind)nx);
	dydi = Ly/((realkind)ny);
	dzdi = Lz/((realkind)nz);

	didx = 1.0/dxdi;
	didy = 1.0/dydi;
	didz = 1.0/dzdi;







	this->setup();

	float mion = 2000.0;



	//printf("num nodes = %i\n",num_nodes);

	for(int i=0;i<NSPECIES_MAX;i++)
	{
		qspecies[i] = q_h[i];
		mspecies[i] = m_h[i];
	}


	for(int i=0;i<NSPECIES_MAX;i++)
	{
		wspecies[i] = n_bh[i]*Lx*Ly*Lz/((double)nptcls_species[i]);

		printf("wspecies[%i] = %e\n",i,wspecies[i]);
	}

	unsigned long long int FNV_prime = 1099511628211ULL;
	unsigned long long int FNV_offset_basis = 14695981039346656037ULL;
	char data_octet[19];
	unsigned long long int hash = FNV_offset_basis;

	int nptcls_t=0;
	for(int i=0;i<nspecies;i++)
		nptcls_t += nptcls_species[i];

	data_octet[0] = nx;
	data_octet[1] = nptcls_t*1.0e-5;
	data_octet[2] = ndimensions+3*nVelocity;
	data_octet[3] = Lx;
	data_octet[4] = nspecies;
	data_octet[5] = node_info->nTasks_g;
	data_octet[6] = node_info->nCPU;
	data_octet[7] = node_info->cpu_info->vec_length;
	data_octet[8] = sizeof(realkind);
	data_octet[9] = dt*10;
	data_octet[10] = nsteps;
	data_octet[11] = rdata->lo_all+1;
	data_octet[12] = device_multiplier[1];
	data_octet[13] = iEM+1;
	data_octet[14] = ny;
	data_octet[15] = nz;
	data_octet[16] = Ly;
	data_octet[17] = Lz;
	data_octet[18] = node_info->nDevices;

	for(int i=0;i<19;i++)
	{
		hash *= FNV_prime;
		hash = hash ^ data_octet[i];
	}

	sprintf(rdata->output_id,"%X",hash);

	timespec res;
	clock_getres(CLOCK_REALTIME,&res);
	tmax = nsteps * dt;

	nreturn = std::min((int)nreturn,node_info->cpu_info->vec_length);

	if(node_info->rank_g == 0){

	// Print out the simulation parameters
	printf("############################################\n");
	printf("Simulation will be run with the following parameters:\n");
	// Number of cells
	printf("\n/********* Spatial Parameters *********/\n");
	printf("Grid Dimensions (nx,ny,nz): %i, %i, %i\n",nx,ny,nz);
	printf("System Lengths (Lx,Ly,Lz): %e, %e, %e\n",Lx,Ly,Lz);
	printf("System Origins (x0,y0,z0): %e, %e, %e\n",xmin,ymin,zmin);

	printf("\n/********* Normalization Parameters *********/\n");
	printf("LO Solver normalization parameter xi: %e\n",xi);
	printf("Normalized Species Charge qe_h, qi_h: %e, %e\n",q_h[0],q_h[1]);
	printf("Physical Species Charge qe_p, qi_p: %e, %e\n",qe_p,qi_p);

	printf("Normalized Species Mass me_h, mi_h: %e, %e\n",m_h[0],m_h[1]);
	printf("Magnetic and Electric Normalization Constants zeta, xi: %e, %e\n",bobzeta,xi);
	printf("eps0_h, mu0_h: %e, %e\n",eps0_h,mu0_h);
	printf("Normalized Temperatures: %e, %e\n",n_bh[0]*q_h[0]*q_h[0]/(m_h[0]*eps0_h),mu0_h);




	printf("\n/********* Particle Parameters *********/\n");
	printf("Number of particles per species: %i\n",nptcls);
	printf("Number of species: %i\n",nspecies);

	printf("\n/********* Time Step Parameters *********/\n");
	printf("Total Simulation Time: %e\n",tmax);
	printf("Time Step Size: %e\n",dt);

	printf("\n/********* Other Parameters *********/\n");
	printf("Relative Tolerance: %e\n",epsilon_r);
	printf("Absolute Tolerance: %e\n",epsilon_a);
	printf("CPU Vector Length: %i\n",node_info->cpu_info->vec_length);
	printf("Number of CPU Cores to Use: %i\n",node_info->nCPU);
	printf("Output ID: %s\n",rdata->output_id);
	printf("Clock resolution: %e (ns)\n",1.0e9*res.tv_sec+res.tv_nsec);
	printf("Sizeof or realkind is %i\n",sizeof(realkind));
	printf("############################################\n");

	}

	output_ctrl = new OutputCtrl(this,argc,argv);




}

PlasmaData::PlasmaData(int argc,char* argv[]) : argc_s(argc),argv_s(argv)
{

	node_info = new ParallelInfo();
	rdata = new RunData();
	rstatus = new RunStatus();

//	system("numactl --nodebind=0,1 --physcpubind=0-15");
	system("numactl --show");

	setvbuf( stdout, NULL, _IONBF, 0 );

	parser = new InputParser(argc,argv);

	thread_info = node_info->init_info(argc,argv);

	// Default Values
	parser->GetParam("NUMBER_CELLS_X","-nx",nx,32);
	parser->GetParam("NUMBER_CELLS_Y","-ny",ny,2);
	parser->GetParam("NUMBER_CELLS_Z","-nz",nz,2);

	parser->GetParam("NUMBER_SPECIES","-ns",nspecies,1);
	parser->GetParam("NUMBER_PARTICLES","-np",nptcls,6400);

	realkind mass_ratio = mi_p1/me_p;
	parser->GetParam("MASS_RATIO","-mr",mass_ratio,mass_ratio);

	mi_p = mass_ratio*me_p;


	for(int i=0;i<node_info->nDevices2;i++)
		printf("iDevice[%i] = %i, igpu = %i\n",i,thread_info[i]->device_id,thread_info[i]->gpu_info->igpu);


	// Check to make sure that nx, ny, and nz are powers of 2
	if(!isPowerOfTwo(nx))
	{	if(node_info->rank_g == 0)
		printf("Error!: nx = %i is not a power of 2\n",nx);
		exit(0);
	}
	else if(!isPowerOfTwo(ny))
	{	if(node_info->rank_g == 0)
		printf("Error!: ny = %i is not a power of 2\n",ny);
		exit(0);
	}
	else if(!isPowerOfTwo(nz))
	{
		if(node_info->rank_g == 0)
		printf("Error!: nz = %i is not a power of 2\n",nz);
		exit(0);
	}


	for(int i=0;i<3;i++)
	{
		nptcls_device[i] = nptcls;
	}

	nptcls_device[1] = 2*nptcls;


//	mion = 100;

	int nptcls_species_temp[NSPECIES_MAX];
	for(int i=0;i<NSPECIES_MAX;i++)
	{
		nptcls_species_temp[i] = 0;
	}

	for(int i=0;i<node_info->nDevices2;i++)
		nptcls_species_temp[thread_info[i]->ispecies] += nptcls_device[thread_info[i]->device_type];

	// Reduce the number of particles for each species
	MPI_Allreduce(nptcls_species_temp,nptcls_species,
			NSPECIES_MAX,MPI_INT,MPI_SUM,MPI_COMM_WORLD);

	for(int i=0;i<NSPECIES_MAX;i++)
	{
		printf("nptcls_species[%i] = %i\n",i,nptcls_species[i]);
	}

	parser->GetParam("PHYSICAL_ELECTRON_DENSITY","-ne_p",ne_p,epsilon_0_p/(qe_p*qe_p));
	parser->GetParam("PHYSICAL_ION_DENSITY","-ni_p",ni_p,epsilon_0_p/(qe_p*qe_p));


	parser->GetParam("MASS_NORMALIZER","-m0",m0,me_p);
	parser->GetParam("CHARGE_NORMALIZER","-q0",q0,qe_p);

	parser->GetParam("DENSITY_NORMALIZER","-n0",n0,ne_p);
	parser->GetParam("PHYSICAL_ELECTRON_TEMPERATURE","-Te_p",Te_p,q0*q0*pow(ne_p,1.0/3.0)/epsilon_0_p);
	parser->GetParam("PHYSICAL_ION_TEMPERATURE","-Ti_p",Ti_p,q0*q0*pow(ni_p,1.0/3.0)/epsilon_0_p);

	parser->GetParam("TEMPERATURE_NORMALIZER","-T0",T0,q0*q0*pow(n0,1.0/3.0)/epsilon_0_p);
	parser->GetParam("LENGTH_NORMALIZER","-L0",L0,sqrt(epsilon_0_p*Te_p/(ne_p*qe_p*qe_p)));

	parser->GetParam("PHYSICAL_SYSTEM_LENGTH_X","-Lx_p",Lx_p,L0);
	parser->GetParam("PHYSICAL_SYSTEM_LENGTH_Y","-Ly_p",Ly_p,L0);
	parser->GetParam("PHYSICAL_SYSTEM_LENGTH_Z","-Lz_p",Lz_p,L0);


	parser->GetParam("NORMALIZED_SYSTEM_LENGTH_X","-Lx",Lx,Lx_p/L0);
	parser->GetParam("NORMALIZED_SYSTEM_LENGTH_Y","-Ly",Ly,Ly_p/L0);
	parser->GetParam("NORMALIZED_SYSTEM_LENGTH_Z","-Lz",Lz,Lz_p/L0);

	parser->GetParam("NORMALIZED_SYSTEM_ORIGIN_X","-x0",xmin,0.0);
	parser->GetParam("NORMALIZED_SYSTEM_ORIGIN_Y","-y0",ymin,0.0);
	parser->GetParam("NORMALIZED_SYSTEM_ORIGIN_Z","-z0",zmin,0.0);

	parser->GetParam("TIME_STEP_SIZE","-dt",dt,0.5);
	parser->GetParam("NUMBER_TIME_STEPS","-s",nsteps,100);
	parser->GetParam("MINIMUM_NUMBER_SUBCYCLES","--min-subcycles",nSubcycle_min,2);
	parser->GetParam("MAXIMUM_INNER_PICARD","--ppicard-max",niter_max,30);


	parser->GetParam("CPU_VECTOR_LENGTH","--vec_length",node_info->cpu_info->vec_length,32);

	parser->GetParam("PARTICLE_PICARD_TOLERANCE","--epsilon",epsilon_r,1.0e-6);
	parser->GetParam("PARTICLE_PICARD_TOLERANCE_A","--epsilon-a",epsilon_a,1.0e-4);


	parser->GetParam("NUMBER_CPU_CORES","--num-cores",node_info->nCPU,32);

	parser->GetParam<float>("GPU_NPTCL_MULTIPLIER","--gpu-mult",node_info->gpu_info->multiplier,1.0f);


	parser->GetParam("PLOT_FLAG","-g",rdata->plot_flag,true);
	parser->GetParam("LO_SOLVE_ALL_NODES","--lo-all",rdata->lo_all,true);
	parser->GetParam("RUNID","--runid",rdata->runid,0);
	parser->GetParam("N_SPATIAL_DIMENSIONS","--nSpatial",ndimensions,1);
	parser->GetParam("N_VELOCITY_DIMENSIONS","--nVel",nVelocity,1);
	parser->GetParam("ELECTROMAGNETIC_FLAG","-iEM",iEM,0);

	parser->GetParam("OUTER_PICARD_TOLERANCE","--epsilon-op",tol_outer_picard,1.0e-6);
	parser->GetParam("MAX_OUTER_PICARD_ITERATIONS","--max-op",nOuterPicard_max,40);

	rdata->SimName = "unknown";
	parser->GetParam("SIMULATION_NAME","--sim-name",rdata->SimName,rdata->SimName);
	pythonScript = new std::string("Viz/Examples/twostream.py");
	parser->GetParam("PYTHON_SCRIPT","--python",*pythonScript,*pythonScript);


	xmin *= Lx;
	ymin *= Ly;
	zmin *= Lz;

	q_h[0] = qe_p/q0;
	q_h[1] = qi_p/q0;
	m_h[0] = me_p/m0;
	m_h[1] = mi_p/m0;
	n_bh[0] = ne_p/n0;
	n_bh[1] = ni_p/n0;
	T_h[0] = Te_p/T0;
	T_h[1] = Ti_p/T0;

	xi = sqrt(epsilon_0_p*T0/(q0*q0*n0*L0*L0));
	bobzeta = m0/(q0*q0*L0*L0*mu_0_p*n0);

//	bobzeta = 1.0;

	nSubcycle_max = 1000;

	eps0_h = xi*xi;
	mu0_h = 1.0/bobzeta;

//	xi = 1.0;
//	bobzeta = 1.0;
//	eps0_h = 1.0;
//	mu0_h = 1.0;


	dxdi = Lx/((realkind)nx);
	dydi = Ly/((realkind)ny);
	dzdi = Lz/((realkind)nz);

	didx = 1.0/dxdi;
	didy = 1.0/dydi;
	didz = 1.0/dzdi;




	for(int i=0;i<node_info->nDevices2;i++)
		printf("iDevice[%i] = %i, igpu = %i\n",i,thread_info[i]->device_id,thread_info[i]->gpu_info->igpu);


	this->setup();

	float mion = 2000.0;



	//printf("num nodes = %i\n",num_nodes);

	for(int i=0;i<NSPECIES_MAX;i++)
	{
		qspecies[i] = q_h[i];
		mspecies[i] = m_h[i];
	}


	for(int i=0;i<NSPECIES_MAX;i++)
	{
		wspecies[i] = n_bh[i]*Lx*Ly*Lz/((double)nptcls_species[i]);

		printf("wspecies[%i] = %e\n",i,wspecies[i]);
	}

	unsigned long long int FNV_prime = 1099511628211ULL;
	unsigned long long int FNV_offset_basis = 14695981039346656037ULL;
	char data_octet[19];
	unsigned long long int hash = FNV_offset_basis;

	int nptcls_t=0;
	for(int i=0;i<nspecies;i++)
		nptcls_t += nptcls_species[i];

	data_octet[0] = nx;
	data_octet[1] = nptcls_t*1.0e-5;
	data_octet[2] = ndimensions+3*nVelocity;
	data_octet[3] = Lx;
	data_octet[4] = nspecies;
	data_octet[5] = node_info->nTasks_g;
	data_octet[6] = node_info->nCPU;
	data_octet[7] = node_info->cpu_info->vec_length;
	data_octet[8] = sizeof(realkind);
	data_octet[9] = dt*10;
	data_octet[10] = nsteps;
	data_octet[11] = rdata->lo_all+1;
	data_octet[12] = device_multiplier[1];
	data_octet[13] = iEM+1;
	data_octet[14] = ny;
	data_octet[15] = nz;
	data_octet[16] = Ly;
	data_octet[17] = Lz;
	data_octet[18] = node_info->nDevices;

	for(int i=0;i<19;i++)
	{
		hash *= FNV_prime;
		hash = hash ^ data_octet[i];
	}

	sprintf(rdata->output_id,"%X",hash);

	timespec res;
	clock_getres(CLOCK_REALTIME,&res);
	tmax = nsteps * dt;

	nreturn = std::min((int)nreturn,node_info->cpu_info->vec_length);

	if(node_info->rank_g == 0){

	// Print out the simulation parameters
	printf("############################################\n");
	printf("Simulation will be run with the following parameters:\n");
	// Number of cells
	printf("\n/********* Spatial Parameters *********/\n");
	printf("Grid Dimensions (nx,ny,nz): %i, %i, %i\n",nx,ny,nz);
	printf("System Lengths (Lx,Ly,Lz): %e, %e, %e\n",Lx,Ly,Lz);
	printf("System Origins (x0,y0,z0): %e, %e, %e\n",xmin,ymin,zmin);

	printf("\n/********* Normalization Parameters *********/\n");
	printf("LO Solver normalization parameter xi: %e\n",xi);
	printf("Normalized Species Charge qe_h, qi_h: %e, %e\n",q_h[0],q_h[1]);
	printf("Physical Species Charge qe_p, qi_p: %e, %e\n",qe_p,qi_p);

	printf("Normalized Species Mass me_h, mi_h: %e, %e\n",m_h[0],m_h[1]);
	printf("Magnetic and Electric Normalization Constants zeta, xi: %e, %e\n",bobzeta,xi);
	printf("eps0_h, mu0_h: %e, %e\n",eps0_h,mu0_h);
	printf("Normalized Temperatures: %e, %e\n",n_bh[0]*q_h[0]*q_h[0]/(m_h[0]*eps0_h),mu0_h);




	printf("\n/********* Particle Parameters *********/\n");
	printf("Number of particles per species: %i\n",nptcls);
	printf("Number of species: %i\n",nspecies);

	printf("\n/********* Time Step Parameters *********/\n");
	printf("Total Simulation Time: %e\n",tmax);
	printf("Time Step Size: %e\n",dt);

	printf("\n/********* Other Parameters *********/\n");
	printf("Relative Tolerance: %e\n",epsilon_r);
	printf("Absolute Tolerance: %e\n",epsilon_a);
	printf("CPU Vector Length: %i\n",node_info->cpu_info->vec_length);
	printf("Number of CPU Cores to Use: %i\n",node_info->nCPU);
	printf("Output ID: %s\n",rdata->output_id);
	printf("Clock resolution: %e (ns)\n",1.0e9*res.tv_sec+res.tv_nsec);
	printf("Sizeof or realkind is %i\n",sizeof(realkind));
	printf("############################################\n");

	}

	output_ctrl = new OutputCtrl(this,argc,argv);





}

void PlasmaData::parse_normalization(int argc, char* argv[])
{
	Lx_p = -1;
	Ly_p = -1;
	Lz_p = -1;
	Te_p = 1.0e-19;
	Ti_p = 1.0e-19;
	ne_p = 1.0e20;
	ni_p = 1.0e20;

	m0 = mi_p;
	q0 = qi_p;
	n0 = 1.0e20;
	T0 = 1.0e-19;
	L0 = -1;


	for(int i=0;i<argc;i++)
		{
			std::string arg(argv[i]);

			if(arg == ("-nx"))
				nx = atoi(argv[i+1]);
			else if(arg == ("-ny"))
				ny = atoi(argv[i+1]);
			else if(arg == ("-nz"))
				nz = atoi(argv[i+1]);

			else if(arg == ("-m0"))
				m0 = atof(argv[i+1]);
			else if(arg == ("-q0"))
				q0 = atof(argv[i+1]);
			else if(arg == ("-L0"))
				L0 = atof(argv[i+1]);
			else if(arg == ("-n0"))
				n0 = atof(argv[i+1]);
			else if(arg == ("-T0"))
				T0 = atof(argv[i+1]);

			else if(arg == ("-Lx"))
				Lx_p = atof(argv[i+1]);
			else if(arg == ("-Ly"))
				Ly_p = atof(argv[i+1]);
			else if(arg == ("-Lz"))
				Lz_p = atof(argv[i+1]);
			else if(arg == ("-Te_p"))
				Te_p = atof(argv[i+1]);
			else if(arg == ("-Ti_p"))
				Ti_p = atof(argv[i+1]);
			else if(arg == ("-ne_p"))
				ne_p = atof(argv[i+1]);
			else if(arg == ("-ni_p"))
				ni_p = atof(argv[i+1]);

			else if(arg == ("-x0"))
				xmin = atof(argv[i+1]);
			else if(arg == ("-y0"))
				ymin = atof(argv[i+1]);
			else if(arg == ("-z0"))
				zmin = atof(argv[i+1]);
		}

	if(L0 == -1)
	{
		L0 = sqrt(epsilon_0_p*Te_p/(ne_p*qe_p*qe_p));
	}

	if(Lx_p == -1)
	{
		Lx_p = L0;
	}

	if(Ly_p == -1)
	{
		Ly_p = L0;
	}

	if(Lz_p == -1)
	{
		Lz_p = L0;
	}

	q_h[0] = qe_p/q0;
	q_h[1] = qi_p/q0;
	m_h[0] = me_p/m0;
	m_h[1] = mi_p/m0;
	n_bh[0] = ne_p/n0;
	n_bh[1] = ni_p/n0;
	T_h[0] = Te_p/T0;
	T_h[1] = Ti_p/T0;

	Lx = Lx_p/L0;
	Ly = Ly_p/L0;
	Lz = Lz_p/L0;

	xi = epsilon_0_p*T0/(q0*q0*n0*L0*L0);


}


void PlasmaData::set_cuda_device()
{
//	device_type = 0;
//#ifndef NO_CUDA
//	if(myrank_node >= 0)
//	{
//		// First get the total number of cuda capable devices
//		int ndevices, ntesla;
//		CUDA_SAFE_CALL(cudaGetDeviceCount(&ndevices));
//
//		int itesla[ndevices];
//
//		// now loop through the devices, we are looking for tesla cards,
//		// The main thing that sets all Tesla cards apart is 2 async engines
//		ntesla = 0;
//		for(int i=0;i<ndevices;i++)
//		{
//			cudaDeviceProp prop;
//			CUDA_SAFE_CALL(cudaGetDeviceProperties(&prop,i));
//			printf("Device[%i] name = %s\n",i,prop.name);
//			std::string name(prop.name);
//			if(name.find("Tesla") != std::string::npos)
//			{
//				itesla[ntesla] = i;
//				ntesla += 1;
//			}
//		}
//
//		if((myrank_node) < ntesla)
//		{
//			device_type = 1;
//			printf("node %i setting device %i\n",myrank_node,itesla[myrank_node-1]);
//			CUDA_SAFE_CALL(cudaSetDevice(itesla[myrank_node-1]));
//		}
//	}
//
////	device_type = 0;
//#endif
}

using namespace std;



void PlasmaData::readInput(const std::string& inFile)
{
  ifstream inStr(inFile.c_str());
  if (!inStr) {
    cout << "Could not open input file " << inFile << endl;
    exit(-1);
  }

  char inBuf[LINESIZE];
  string keyword;
  string rest;

  while (inStr.getline(inBuf, LINESIZE)) {
    if (inBuf[0] != '#' && inStr.gcount() > 1) {

      getKeyword(inBuf, keyword, rest);
      istringstream line(rest.c_str());
      cout << "KEYWORD: " << keyword << "   " << rest << endl;

      /*
      if (keyword == "PROBLEM_TYPE")
        line >> this->prob_type;
      else if (keyword == "PROBLEM_CASE")
        line >> this->prob_case;
      else if (keyword == "QUIET_START")
        line >> this->quiet_start_flag;
      else if (keyword == "SYSTEM_LENGTH")
        line >> this->lx;
      else if (keyword == "NUMBER_OF_POS_CELLS")
        line >> this->nx;
      else if (keyword == "NUMBER_OF_VEL_CELLS")
        line >> this->nv;

      else if (keyword == "TIME_MAX")
        line >> this->tmax;
      else if (keyword == "TIME_STEP")
        line >> this->dt;

      else if (keyword == "NUMBER_OF_FILTERING_OPERATIONS")
        line >> this->fil_num;
      else if (keyword == "CHARGE_CONSERVE_FLAG")
        line >> this->cc_flag;
      else if (keyword == "LO_SOLVER")
        line >> this->si_lo_flag;
      else if (keyword == "TEMPORAL_ORDER")
        line >> this->temp_order;
      else if (keyword == "W_SCALE")
        line >> this->w_scale;

      else if (keyword == "NUMBER_OF_PARTICLE_SPECIES")
        line >> this->p_size;
      else if (keyword == "PARTICLE_TYPE")
        line >> this->p_type[0] >> this->p_type[1];
      else if (keyword == "PARTICLE_REFERENCE_NUMBER")
        line >> this->NP_ref[0] >> this->NP_ref[1];
      else if (keyword == "SPECIES_CHARGE")
        line >> this->q[0] >> this->q[1];
      else if (keyword == "SPECIES_MASS")
        line >> this->m[0] >> this->m[1];
      else if (keyword == "SPECIES_SUBCYCLE")
        line >> this->sub_nt[0] >> this->sub_nt[1];

      else if (keyword == "DENSITY_UNPERTURBED")
        line >> this->rho0[0] >> this->rho0[1];
      else if (keyword == "VELOCITY_UNPERTURBED")
        line >> this->u0[0] >> this->u0[1];
      else if (keyword == "TEMPERATURE_UNPERTURBED")
        line >> this->T0[0] >> this->T0[1];
      else if (keyword == "DENSITY_PERTURBED")
        line >> this->alp_r[0] >> this->alp_r[1];
      else if (keyword == "VELOCITY_PERTURBED")
        line >> this->alp_u[0] >> this->alp_u[1];
      else if (keyword == "TEMPERATURE_PERTURBED")
        line >> this->alp_T[0] >> this->alp_T[1];
        */



      if (keyword == "SYSTEM_LENGTH_X")
        line >> this->Lx;
      else if (keyword == "SYSTEM_LENGTH_Y")
        line >> this->Ly;
      else if (keyword == "SYSTEM_LENGTH_Z")
        line >> this->Lz;

      else if (keyword == "NUMBER_OF_POS_CELLS_X")
        line >> this->nx;
      else if (keyword == "NUMBER_OF_POS_CELLS_Y")
        line >> this->ny;
      else if (keyword == "NUMBER_OF_POS_CELLS_Z")
        line >> this->nz;

      else if (keyword == "X_ORIGIN")
        line >> this->xmin;
      else if (keyword == "Y_ORIGIN")
        line >> this->ymin;
      else if (keyword == "Z_ORIGIN")
        line >> this->zmin;

      else if (keyword == "NUMBER_OF_TIME_STEPS")
        line >> this->nsteps;
      else if (keyword == "TIME_STEP")
        line >> this->dt;

      else if (keyword == "NUMBER_OF_PARTICLES")
        line >> this->nptcls;
      else if (keyword == "NUMBER_OF_PARTICLE_SPECIES")
        line >> this->nspecies;

      else if (keyword == "CPU_VECTOR_LENGTH")
        line >> this->node_info->cpu_info->vec_length;
      else if (keyword == "PICARD_TOLERANCE")
        line >> this->epsilon_r;


    }
  }
}

void PlasmaData::getKeyword(char* inBuf, string& keyword, string& rest)
{
  string line(inBuf);
  string::size_type keyPos = line.find(' ');
  keyword = line.substr(0, keyPos);
  rest = line.substr(keyPos + 1);
}

__attribute__ ((noinline))
InputParser::InputParser(int _argc, char** _argv) : argc(_argc), argv(_argv)
{
	iFile = 0;
	fileOpen = 0;
	// Check for an input file
	for(int i=0;i<argc;i++)
	{
		std::string arg(argv[i]);
		if(arg == ("-f"))
		{
			// Need to read the input file first
			inFile = new std::string(argv[i+1]);
			iFile = 1;
			fileOpen = 1;
			inStr = new ifstream(inFile->c_str());
			if (!inStr) {
				cout << "Could not open input file " << inFile << endl;
				exit(-1);
			}
			break;

		}
	}

}


//template<typename T> __attribute__((noinline))
//void InputParser::GetParam(const char* 	fileKey, // Keyword for value in an Input File
//						   const char* 	comKey, // Keyword for a value in the command line
//						   T& 		output, // value to write
//						   T 		defVal) // Default value
//{
//	output = defVal;
//
//	if(iFile == 1)
//	{
//		// Open the file if it got closed
//		if(fileOpen != 1)
//		{
//			fileOpen = 1;
//			inStr = new ifstream(inFile->c_str());
//			if (!inStr) {
//				cout << "Could not open input file " << inFile << endl;
//				exit(-1);
//			}
//		}
//
//		// Set the file buffer to the beginning of the file
//		inStr->clear();
//		inStr->seekg(0, ios::beg);
//
//		char inBuf[LINESIZE];
//		string keyword;
//		string rest;
//
//		while (inStr->getline(inBuf, LINESIZE)) {
//			if (inBuf[0] != '#' && inStr->gcount() > 1) {
//
//				getKeyword(inBuf, keyword, rest);
//				istringstream line(rest.c_str());
////				cout << "KEYWORD: " << keyword << "   " << rest << endl;
//
//				if (keyword == fileKey)
//					line >> output;
//			}
//		}
//	}
//
//	// Command Line input overwrites file input
//	for(int i=0;i<argc;i++)
//	{
//		std::string arg(argv[i]);
//
//		if(arg == comKey)
//			istringstream(argv[i+1]) >> output;
//	}
//
//	cout << string(fileKey) << string(" = ") << (output) << endl;
//
//
//}

__attribute__ ((noinline))
void dummy_func(int argc, char** argv)
{
	InputParser temp(argc,argv);
	float temp2;

	temp.GetParam("stupid","stupid",temp2,1.0f);


}

__attribute__ ((noinline))
void InputParser::getKeyword(char* inBuf, string& keyword, string& rest)
{
  string line(inBuf);
  string::size_type keyPos = line.find(' ');
  keyword = line.substr(0, keyPos);
  rest = line.substr(keyPos + 1);
}














