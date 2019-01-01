// Utilisation des pragma pour activer les extensions atomic;
#pragma OPENCL EXTENSION cl_khr_local_int32_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomics : enable
// Structure du Kd-tree
typedef  struct {
	float4 Pmin;
	float4 Pmax;
	unsigned int BI;
	unsigned int Visite;
	unsigned int dummy;
	unsigned int dummy2;
} LigneKdGPU;

/*----------------------------------------------------------*/
// Kernels des tris Bitonic sur X et Y
// A améliorer. TODO, regrouper en un seul en passant la dimension de coupure.
// On supprime au moins un kernel !

inline void OrdonneX(__global float4 *data, uint val1, uint val2, uint dir)
{
	if( (data[val1].x > data[val2].x) == (dir==0) )
	{
	    float4 t;
	    t = data[val1]; data[val1] = data[val2]; data[val2] = t;
    }
}
__kernel void BiToniX(__global float4 *data, const uint tailleB, const uint decal, const uint pk) 
{
 unsigned int id = get_global_id(0);
 unsigned int pos = (id&(decal-1))+ 2*decal*(id/decal);
 unsigned int dir = ((id/pk)&1);
 OrdonneX(data,pos,pos+decal,dir);
 
}

/*----------------------------------------------------------*/

inline void OrdonneY(__global float4 *data, int val1, int val2, int dir,__global uint *RangId)
{
	if( (data[val1].y > data[val2].y) == (dir==0) )
	{
        float4 t;
	    t = data[val1]; data[val1] = data[val2]; data[val2] = t;
		uint tt;
		tt = RangId[val1]; RangId[val1] =  RangId[val2]; RangId[val2] = tt;
    }
}
__kernel void BiToniY(__global float4 *data, const uint tailleB, const uint decal, const uint pk,__global uint *RangId) 
{
 int id = get_global_id(0);
 int pos = (id&(decal-1))+ 2*decal*(id/decal);
 int dir = ((id/pk)&1);
 OrdonneY(data,pos,pos+decal,dir,RangId);
 
}
/*----------------------------------------------------------*/
// Kernel d'affectation des rangs une fois le tableau trié
// On prend le numéro du point qui est dans DataIn[i].w, la 4ième valeur
// Et BBO du Kd
__kernel void Rang_TriX(__global float4 *DataIn,__global uint *Rang, __global LigneKdGPU *Kd,uint Nb)
{
	unsigned int id = get_global_id(0);
	Rang[id] = id;
	if (id==0)
	{
		Kd[0].Pmin.x = DataIn[0].x;
		Kd[0].Pmax.x = DataIn[Nb-1].x;
	}
}

__kernel void Rang_TriY(__global float4 *DataIn,__global uint *Rang,__global uint *RangId, __global LigneKdGPU *Kd,uint Nb)
{
	unsigned int id = get_global_id(0);
	Rang[id] = id;
	RangId[id] = id;
	if (id==0)
	{
		Kd[0].Pmin.y = DataIn[0].y;
		Kd[0].Pmax.y = DataIn[Nb-1].y;
	}
	
}

/*----------------------------------------------------------*/
// Kernel 5
__kernel void Flag_RaZ( __global uint *V1,__global uint *V2,
						__global uint *V3,__global uint *V4)
{
	unsigned int id=get_global_id(0);
	V1[id]=0;	V2[id]=0;
	V3[id]=0;	V4[id]=0;
}

/*----------------------------------------------------------*/
// Kernel 6
__kernel void Flag_InitD(	__global uint *Rang,
							__global uint *VectFlag,
							uint mediane,
							uint debut)
{
	unsigned int id = get_global_id(0);
	unsigned int rang = Rang[id+debut];
	VectFlag[id] = (rang < mediane); 

}

/*----------------------------------------------------------*/
// Kernel 7
__kernel void ScanGPU(	__global uint *VectFlag,
						int NbE,
						__global int *SumPrefix ,
						__global int *SumPrefixN, 
						__local int *temp,
						__local int *temp2)
{
	unsigned int idloc = get_local_id(0);
	unsigned int idT_2 = get_global_id(0) << 1;
	unsigned int idloc_2 = get_local_id(0) << 1;
	
	int offset = 1;
	
	temp[idloc_2] = VectFlag[idT_2]; 
	temp[idloc_2+1] = VectFlag[idT_2 + 1];
	
	temp2[idloc_2] = 1 - temp[idloc_2]; 
	temp2[idloc_2 + 1] = 1 - temp[idloc_2 + 1];
	

	for (int d = NbE>>1; d > 0; d >>= 1)
	{
		barrier(CLK_LOCAL_MEM_FENCE);
		if (idloc < d)
		{
			int ai = offset*(idloc_2 + 1) - 1;
			int bi = offset*(idloc_2 + 2) - 1;
			temp[bi] += temp[ai];
			temp2[bi] += temp2[ai];
		}
		offset <<= 1;
	}
		
	if (idloc == 0) 
	{ 
		temp[NbE - 1] = 0;
		temp2[NbE - 1] = 0;
	}
		
	for (int d = 1; d < NbE; d <<= 1)
	{
		offset >>= 1;
		barrier(CLK_LOCAL_MEM_FENCE);
		if (idloc < d)
		{
			int ai = offset*(idloc_2 + 1) - 1;
			int bi = offset*(idloc_2 + 2) - 1;
			int t = temp[ai];
			int t2 = temp2[ai];
			
			temp[ai] = temp[bi];
			temp2[ai] = temp2[bi];
			
			temp[bi] += t;
			temp2[bi] += t2;
		}
	}
	barrier(CLK_LOCAL_MEM_FENCE);
	
	SumPrefix[idT_2] = temp[idloc_2];
	SumPrefix[idT_2 + 1] = temp[idloc_2 + 1];
	
	SumPrefixN[idT_2] = temp2[idloc_2];
	SumPrefixN[idT_2 + 1] = temp2[idloc_2 + 1];
	
}
/*----------------------------------------------------------*/
// Kernel 8
// Affecte les valeurs des sommes prefixes à VectBloc
// Attention si la dernière valeur du bloc d'avant dans les flags est 1
// Il faut augmenter de 1 la valeur stockée
__kernel void Aff_Bloc(	__global int *SumPrefix ,
						__global int *SumPrefixN,
						__global int *VectBloc,
						__global int *VectBlocN,
						__global uint *VectFlag )
{
	unsigned int idBloc	= get_global_id(0);
	unsigned int pos = (Max*(idBloc+1))-1;
	VectBloc[idBloc] = SumPrefix[pos] + VectFlag[pos];
	VectBlocN[idBloc] = SumPrefixN[pos] + 1 - VectFlag[pos];
}
/*----------------------------------------------------------*/
// Kernel 9
__kernel void SumBloc( int NbE,
						__global uint *VectBloc,
						__global uint *VectBlocN,
						__local uint *temp,
						__local uint *temp2	)
{
// Seconde Partie, On effectue un nouveau scan sur VectBloc
// Cette fois-ci, on estime qu'il y a au max 1024 elements
	unsigned int id = get_local_id(0);
	unsigned int id_2 = get_local_id(0) << 1;
	int offset = 1;
	
	temp[id_2]  = VectBloc[id_2];
	temp[id_2 + 1]= VectBloc[id_2 + 1];
	
	temp2[id_2]  = VectBlocN[id_2]; 
	temp2[id_2 + 1]= VectBlocN[id_2 + 1];
		
	for (int d = NbE>>1; d > 0; d >>= 1)
	{
		barrier(CLK_LOCAL_MEM_FENCE);
		if (id < d)
		{
			int ai = offset*(id_2 + 1) - 1;
			int bi = offset*(id_2 + 2) - 1;
			temp[bi] += temp[ai];
			temp2[bi]+= temp2[ai];
		}
		offset <<= 1;
	}
	
	if (id == 0) 
	{ 
		temp[NbE - 1] = 0; 
		temp2[NbE - 1] = 0; 
	}
	
	for (int d = 1; d < NbE; d <<= 1)
	{
		offset >>= 1;
		barrier(CLK_LOCAL_MEM_FENCE);
		if (id < d)
		{
			int ai = offset*(id_2 + 1)-1;
			int bi = offset*(id_2 + 2)-1;
			int t = temp[ai];
			int t2 = temp2[ai];
			
			temp[ai] = temp[bi];
			temp2[ai] = temp2[bi];
			
			temp[bi] += t;
			temp2[bi] += t2;
		}
	}
	barrier(CLK_LOCAL_MEM_FENCE);

	VectBloc[id_2] = temp[id_2];
	VectBloc[id_2 + 1] = temp[id_2 + 1];
	
	VectBlocN[id_2] = temp2[id_2];
	VectBlocN[id_2 + 1] = temp2[id_2 + 1];
	
}
/*----------------------------------------------------------*/
// On repercute la somme prefixe de VectBloc dans SumPrefix
// Kernel 10
__kernel void Prefix_Bloc(	__global uint *SumPrefix ,
							__global uint *SumPrefixN,
							__global uint *VectBloc,
							__global uint *VectBlocN )
{
	unsigned int id = get_global_id(0);
	unsigned int idBloc = id / Max;

	SumPrefix[id] += VectBloc[idBloc];
	SumPrefixN[id] += VectBlocN[idBloc];
	
}
/*----------------------------------------------------------*/
// Kernel 11
// Reparti dans la première partie du tableau les élements < mediane (sinon dans la deuxième partie)
// dont les indices (positions) ont été calculées dans les sommes prefixes S et SN
// La variable debut correspond au premier indice à modifier
// On en profite pour mettre à jour le Kd
__kernel void Reparti(	__global uint *SumPrefix,
						__global uint *SumPrefixN,
						__global uint *RangIn,
						__global uint *RangOut,
						__global uint *RangId,
						__global float4 *Data,
						__global LigneKdGPU *Kd,
							uint mediane,
							uint debut,
							uint dimension,
							uint numKd,
						__global uint *Interchange)
{
	unsigned int id = get_global_id(0);
	unsigned rk = RangIn[ debut + id];
	
	if (rk < mediane)
	{
		RangOut[ debut + SumPrefix[id] ] = rk;
		Interchange[debut+id] = debut + SumPrefix[id];
			
		if (rk == (mediane-1))
		{
			Kd[numKd].dummy = RangId[debut+id];
			Kd[numKd+1].dummy = RangId[debut+id];
		}
	}
	else
	{
		unsigned int pos = SumPrefixN[id] + mediane;
		RangOut[ debut + pos ]=  rk;
		Interchange[debut+id] = debut + pos;
	
		if (rk == mediane)
		{
			Kd[numKd].dummy2 = RangId[debut+id];
			Kd[numKd+1].dummy2 = RangId[debut+id];
		}
	}

	barrier(CLK_GLOBAL_MEM_FENCE);

	if (id == 0)
	{
		unsigned int idPere = ( numKd - 1 ) / 2;
		float4 PminP = Kd[idPere].Pmin;
		float4 PmaxP = Kd[idPere].Pmax;
		float D1;
		float D2;
		float moyenne;
		unsigned int id1 = Kd[numKd].dummy;
		unsigned int id2 = Kd[numKd].dummy2;
		
		// Mise à jour du Kd
		Kd[numKd].Pmin = PminP;
		Kd[numKd].Pmax = PmaxP;
		Kd[numKd+1].Pmin = PminP;
		Kd[numKd+1].Pmax = PmaxP;
				
		if (dimension == 0)
		{
				D1 = Data[id1].x;
				D2 = Data[id2].x;
				moyenne = 0.5*(D1+D2);
				
				Kd[numKd].Pmax.x = moyenne;
				Kd[numKd].Pmax.y = PmaxP.y;
				
				Kd[numKd+1].Pmin.x = moyenne;
				Kd[numKd+1].Pmin.y = PminP.y;

			}else
			{
				D1 = Data[id1].y;
				D2 = Data[id2].y;
				moyenne = 0.5*(D1+D2);
				
				Kd[numKd].Pmax.y = moyenne;
				Kd[numKd].Pmax.x = PmaxP.x;
				
				Kd[numKd+1].Pmin.y = moyenne;
				Kd[numKd+1].Pmin.x = PminP.x;
			}			
	}

}
/*----------------------------------------------------------*/
// Kernel 12
// Echange deux vecteurs de rangs
__kernel void Echange(__global uint *Vecteur1, __global uint *Vecteur2, uint debut)
{
	unsigned int id = debut + get_global_id(0);
	unsigned int temp = Vecteur1[id];
	Vecteur1[id] = Vecteur2[id];
	Vecteur2[id] = temp;
	
}
/*----------------------------------------------------------*/
// Kernel 13
// Affectation des flags pour les dimensions autres que la dimension de coupure
__kernel void Repercute_Init(	__global const uint *RangIn,
								__global int *VectFlag,
									uint debut)
{	
	unsigned int id = debut + get_global_id(0);
	VectFlag[RangIn[id]]=1;
	
}
/*----------------------------------------------------------*/
// Kernel 14 Renumérotation des Rangs à l'aide de SumPrefix
__kernel void Renum(	__global uint *RangIn,
						__global uint *SumPrefix ,
						__global uint *SumPrefixN ,
						__global uint *RangOut, 
						uint debut,
						uint debutM)
{
	unsigned int id = get_global_id(0);
	RangOut[id+debut]  = SumPrefix [RangIn[id+debut]];
	RangOut[id+debutM] = SumPrefixN[RangIn[id+debutM]];
}
/*----------------------------------------------------------*/
// Kernel 15 Soustrait la valeur passée à tous les elements du tableau
__kernel void Soustrait(__global uint *RangIn,uint valeur, uint debut)
{
	unsigned int id = debut + get_global_id(0);
	RangIn[id] = RangIn[id] - valeur;
 
}
/*----------------------------------------------------------*/
// Kernel 16 Echange les positions d'un vecteur de rang en fonction de la valeur de interchange
// Fait suivre les valeurs déplacées suite à la coupure
__kernel void Suivre( __global  uint *RangIn, __global  uint *RangOut, __global  uint *Interchange, uint debut)
{
	unsigned int id = debut + get_global_id(0);
	RangOut[Interchange[id]] = RangIn[id];
	
}
/*----------------------------------------------------------*/

