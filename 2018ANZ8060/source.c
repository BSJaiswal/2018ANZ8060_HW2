//-------------------------------------
//C program to compute k means clusters
//-------------------------------------
//It takes <input.dat>  as input file and value 'k' as parameter at run time
//data point is upto 5 dimensions 
//feature value is float
//size of dataset is upto 1 m
//
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h> 
#include <unistd.h>

int iteration =0;

// #1: function for reading input
static int read_next_data(int fd, float *data)
{
	static char c;
	int num_read;
	char float_string[50]; /*maximum size of float_string is 50 (assumed) */
	int count = 0;
	float float_data = 0;

	/* ignore all non float char */
	while ((c < '0' || c > '9') && (c != '.') && (c != '-')) 
	{
		num_read = read(fd, &c, 1);
		/* If we could not read even a single char then it's EOF */
		if (num_read != 1)
			return 0;//blank file
	}
	/*
	 * Now, keep on reading until we find a char not matching a float
	 */
	do {
		float_string[count++] = c;
		read(fd, &c, 1);
	} while ((c >= '0' && c <= '9') || (c == '.') || (c == '-'));

	/* convert string into integer */

	float_data = atof(float_string);
	*data = float_data;


	/* check for new line or new float */
	while ((c < '0' || c > '9') && (c != '.') && (c != '-')) 
	{
		if (c == '\n')
			return 2;//EOL
		num_read = read(fd, &c, 1);
		/* If we could not read even a single char then it's EOF */
		if (num_read != 1)
			return 0;//blank file
	}
	return 1;
}


//#2 : function for Computation of Centroid
void compute_centroid(int d, int n, int k, float *X, int *cluster_linkage_update, float *new_cent)
  {
    int cluster_strength[1000000];
	 int tempc =0;
   // initialization of zero value
    for (int j = 0; j < k; j++) 
      {
      cluster_strength[j] = 0;
      for (int m = 0; m < d; m++)
      new_cent[j*d + m] = 0;
      }
   // summation over cluster points
	float sum[5];
	int r, out=0;
	for (r=0; r<5; r++)
	sum[r]=0;
    for (int i = 0; i < n; i++)
      {
      int tempc = cluster_linkage_update[i];
      cluster_strength[tempc]++;
      for (int m = 0; m < d; m++)
	  {
	  if (tempc != -1)
		{
      new_cent[tempc*d + m] += X[i*d + m];
		sum[m] +=X[i*d + m];
		}
		else
		{
		out++;
		}
	  }
     }
		//printf("#out is %d\n", out);
	//printing sum of overall centroid
	//printf("summation of centroids\n");
	for (int v=0; v<5; v++)
	{
	sum[v] /= n;
	//printf("%f\t", sum[v]);
	}
	//printf("\n");

	//centroid = summation/count
	//printf("centroids after this round is:\n");
    for (int j = 0; j < k; j++) 
      {
        for (int m = 0; m < d; m++)
		  {
		  		if (cluster_strength[j] != 0)
				{
        		new_cent[j*d + m] /= cluster_strength[j];
				//printf("%.10f\t", new_cent[j*d + m]);
		  		}
				else
				{
				new_cent[j*d + m] =sum[m];
				sum[m] *= 0.8;
				}
      }
		//printf("\n");
  }
}

//#3 : function to find square of Euclidean Distance between 2 points

float Edist (int d, float *Pt1, float *Pt2)
  {
    float temp = 0;
    for (int m = 0; m < d; m++)
    temp += pow((Pt1[m]-Pt2[m]),2);
    return temp;
  }

//#4 : function to find square of Euclidean Distance between all points and their respective cluster centroids

void dist_fm_cent (int d, int n, int k, float *X, float *cent, float *Edist_ij)
  {
    for (int i = 0; i < n; i++)
	{
      for (int j = 0; j < k; j++)
		{
      Edist_ij[i*k + j] = Edist(d, &X[i*d], &cent[j*d]);
		//printf("%.1f\t", Edist_ij[i*k + j]);
		}
		//printf("\n");
	}
  }
  
//#5 : Rearrangement of clusters based on Euclidean distance criterion
void rearrange_cluster(int d, int n, int k, float *dist_table, int *cluster_linkage_update)
  {
	float temp;
	int tempi;
    for (int i = 0; i < n; i++)
      {
        int temp_index = -1;//no assignment
        float ref = 1.0e100;//a very large number
        for (int j = 0; j < k; j++)
          {
          temp = dist_table[i*k + j];
          if (temp < ref)
             {
             temp_index = j;
             ref = temp;
             }
          }
        cluster_linkage_update[i] = temp_index;
		tempi += tempi;
		//printf("%d\t%d\n",i,cluster_linkage_update[i]);
      }
	iteration++;
	//printf("%d  iteration/s completed\n", iteration);
  }


 //#6 :function for finding number of changes in the elements of previous and current arrays
int delta_count(int n, int a[], int b[])
  {
    int differential_count = 0;
    for (int i = 0; i < n; i++)
    if (a[i] != b[i])
    differential_count++;   
    return differential_count;
  }

//#7 : function for copying an array element wise
void memcopy(int n, int *source, int *target)
  {
    for (int i = 0; i < n; i++)
      target[i] = source[i];
  }

//main function for k means algorithm
int main(int argc, const char *argv[])
{
	int  d,n,k,c,p;
	float *X;
	X = (float *)malloc(sizeof(float) * 5000000);
	float cent[10000];
	float *E_dist;
	int   *run_cref;
	int   *prev_cref;
	char *input;
	int fd, flag, round, i, j, m;
	float data;
	int max_iter= 2; //maximum number of iterations
	if (argc < 2) {
		printf("Provide 1st argument as value of k\n");
		printf("Provide 2nd argument as name of input file\n");
		return -1;
	}
	k=atoi(argv[1]);// k taken as input parameter
	//opening input.dat to read n (<=5) dim data points; data size<= 1 million
	if (argv[2])
		fd=open(argv[2], O_RDONLY);
	else
		fd=open("input.dat", O_RDONLY);
	if (!fd) 
	{
		printf("error in opening input.dat\n");
		exit(1);
	}
	d=0;//dimensions
	i=0;//no of lines
	p=0;//partitions
	for (int m=0; m<5; m++)
	{
		flag = read_next_data(fd, &data);
		X[i]=data;
		i++;
		d++;
		if (flag ==2) break;
	}
	//printf("no of dimensions is %d\n",d);
	//printf("no of values in X is %d\n",i);
	flag = read_next_data(fd, &data);
	do
	{	
		X[i]=data;
		i++;
		flag = read_next_data(fd, &data);
	} while (flag != 0);
	close(fd);
	//data reading complete
	//printf("no of values in X is %d\n",i);
	n=ceil(i/d);
	//printf("no of points is %d\n",n);
	E_dist = (float *)malloc(sizeof(float) * n * k);
	run_cref = (int *)malloc(sizeof(int) * n);
	prev_cref = (int *)malloc(sizeof(int) * n);

# if 0
//printing of input data:  for testing purposes only
	int count1 =0;
	for (j=0; j<n; j++)
	{
	printf("%d\t", j);
	for (m=0; m<d; m++)
	{
	printf("%.10f",X[j*d +m]);
	printf("\t");
	}
	printf("\n");
	count1++;
	}
	//printf("number of lines printed is %d",count1);
# endif

	//initial centroids
	p=floor(n/k);
	//printf("initially assumed centroids are:\n");
	for (c=0; c<k; c++)
	{	
		
		for (m=0; m<d; m++)
		{
		cent[c*d +m] = X[c*p*d+m];
		//printf("%.10f",cent[c*d +m]);
		//printf("\t");
		}
	}

	//square of Euclidean distance: points to centroids
	dist_fm_cent(d, n, k, X, cent, E_dist);

	//cluster allocation on the basis of Euclidean distance
	rearrange_cluster(d, n, k, E_dist, run_cref); 
	memcopy(n, run_cref, prev_cref);

	// Iteration of kmeans algorithm
	round= 1;
	while (round <=100 )// max of iterations taken as 100
	{
		compute_centroid(d, n, k, X, run_cref, cent);
		dist_fm_cent(d, n, k, X, cent, E_dist);
		rearrange_cluster(d, n, k, E_dist, run_cref);
		round++;
		if (delta_count(k, run_cref, prev_cref)==0) //if cluster members remain the same
		{
			//printf("kmeans converged after round-\t%d\n",round);
			break;
		}
		memcopy(n, run_cref, prev_cref);
	}


//output file <kmeans.txt> : #<cluster ID> followed by one cluster member per line representaed as <line ref number> of input dataset 
FILE *fp;
fp=fopen("kmeans.txt", "w");
if (!fp) 
	{
	printf("error in opening output.txt\n");
	exit(1);
	}
int clust=0;
fprintf(fp, "#%d", clust);
while (clust < k)
{
	for (i=0; i<n; i++) 
	{
	if (run_cref[i]==clust)
	fprintf(fp, "\n%d ", i);
	}
	clust++;
	if (clust < k)
	fprintf(fp, "\n#%d", clust);
}
fclose(fp);  
free(E_dist);
free(run_cref);
free(prev_cref);
free(X);
}
