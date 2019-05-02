/** Implementation of Nohl's algorithm for Pharaoh's Golden Staircase       **
 ** problem:                                                                **
 ** Daniel E. Nohl, "Solving the Pharaoh's Golden Staircase Problem through **
 ** Dynamic Programming", CCSC: Midwestern Conference, pp 54--60, 2004.     **
 **                                                                         **
 ** Implemented by Abhijit Das on 15-Oct-2014.                              **/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NDFT 20
#define MDFT 10
#define MAXWD 5
#define MAXHT 5
#define INFINITY 1234567890

/* Generate n steps of random widths and heights */
void initSteps ( int n, int *W, int *H )
{
   int i;

   for (i=0; i<n; ++i) {
      W[i] = 2 + rand() % (MAXWD - 1);
      H[i] = 2 + rand() % (MAXHT - 1);
   }
}

/* Prepare the merge-cost array M[][]. n is the number of steps. W[] and H[]
   store the widths and heights of the steps. M[i][j] stores the cost of
   merging Steps i through j, where i <= j. */
void calcMergeCosts ( int n, int *W, int *H, int **M )
{
   int i, j, k, sum;

   for (i=0; i<n; ++i) {
      M[i][i] = 0; /* No merging required */
      for (j=i+1; j<n; ++j) {
         /* Compute the total contribution (width) due to the introduction of
            the jth step */
         sum = 0;
         for (k=i; k<j; ++k) sum += W[k];

         /* Add this contribution to M[i][j-1] after multiplying by height */
         M[i][j] = M[i][j-1] + H[j] * sum;
      }
   }

   printf("+++ Array of merging costs:\n");
   for (i=0; i<n; ++i) {
      for (j=0; j<n; ++j) {
         if (i > j) printf("     "); else printf("%5d", M[i][j]);
      }
      printf("\n");
   }
}

/* This function computes the optimal cost of replacing n controllers by
   m controllers, where m <= n. The merge-cost array M[][] is also passed
   as an input argument. The two-dimensional array T[][] stores the
   optimum costs. More precisely, T[i][j] stores the optimum cost of
   replacing the FIRST i+1 controllers C_0 to C_i by j+1 controllers
   C_0 to C_j, where j <= i. The other output array S[][] stores the
   solution, that is, S[i][j] stores the index where the last controller
   C_j should be placed so that the restructuring cost is T[i][j].
   The desired output is stored at T[n-1][m-1]. */
void calcOptimalSoln ( int n, int m, int **M, int **T, int **S )
{
   int i, j, thiscost, k;

   /* Replacing C_0 to C_i by C_0 to C_i calls for no cost */
   for (i=0; i<m; ++i) {
      T[i][i] = 0; /* Zero cost */
      S[i][i] = i; /* Last controller at last index */
   }

   /* Replacing C_0 to C_i by only one controller C_0 is the cost of nerging
      all the steps 0 through i */
   for (i=1; i<n; ++i) {
      T[i][0] = M[0][i]; /* Merging cost */
      S[i][0] = 0;       /* The only controller is to be placed at index zero */
   }

   /* Now iteratively fill the rest of the table in a column-major fashion */
   for (j=1; j<m; ++j) {

      /* i should be >= j. The case i = j is already handled */
      for (i=j+1; i<n; ++i) {

         /* Initialize the minimum cost */
         T[i][j] = INFINITY;

         /* Look at all valid positions k for putting the last controller. The
            first allowed position of C_j should be such that there are just
            enough steps before it to put the j controllers C_0 to C_(j-1).
            The last allowed position is clearly i (implying no merging). */
         for (k=j; k<=i; ++k) {

            /* Compute the minimum cost if C_j is placed at index k.
               That means C_0 through C_(j-1) should be placed at indices
               0 through k-1. Moreover, steps k through i should be merged. */
            thiscost = T[k-1][j-1] + M[k][i];

            /* Check whether a lower merging cost is encountered */
            if (thiscost < T[i][j]) {
               T[i][j] = thiscost; /* Store the lower cost */
               S[i][j] = k;        /* Remember the position k of the controller */
            }
         }
      }
   }

   printf("+++ Array of optimal costs:\n");
   for (i=0; i<n; ++i) {
      for (j=0; j<m; ++j) {
         if (i < j) printf("     ");
         else printf("%5d", T[i][j]);
      }
      printf("\n");
   }

   printf("+++ Array of optimal solutions:\n");
   for (i=0; i<n; ++i) {
      for (j=0; j<m; ++j) {
         if (i < j) printf("     ");
         else printf("%5d", S[i][j]);
      }
      printf("\n");
   }
}

/* Convert the solution stored in S[][] to the new sequence of steps */
void calcNewSteps ( int n, int m, int *W, int *H, int **S, int *newW, int *newH )
{
   int i, j, k, t;

   /* The new sequence of steps is constructed backward, that is, from
      top to bottom */

   k = m - 1; i = n - 1; j = m - 1;

   while (k >= 0) {
      newW[k] = newH[k] = 0;
      for (t = S[i][j]; t <= i; ++t) { /* Merge steps S[i][j] through i */
         newW[k] += W[t]; newH[k] += H[t];
      }
      i = S[i][j] - 1; --j; --k;       /* Prepare for the next merging */
   }
}

/* Funcion for a text-based drawing of the steps. This works under the
   assumption that the widths and heights of the steps are positive
   integers. The problem makes sense even if these are floating-point
   numbers, but then drawing the steps requires more effort. */
void drawSteps ( int n, int *W, int *H )
{
   char **A;
   int totw, toth;
   int i, j, s, t;

   printf("--- Widths : "); for (i=0; i<n; ++i) printf("%3d", W[i]); printf("\n");
   printf("--- Heights: "); for (i=0; i<n; ++i) printf("%3d", H[i]); printf("\n");
   for (i=totw=toth=0; i<n; ++i) {
      totw += W[i]; toth += H[i];
   }

   A = (char **)malloc((toth+1) * sizeof(char *));
   for (i=0; i<=toth; ++i) {
      A[i] = (char *)malloc((totw + 2) * sizeof(char));
      for (j=0; j<totw; ++j) A[i][j] = ' ';
      A[i][totw] = '|';
      A[i][totw+1] = '\0';
   }
   s = t = 0;
   for (i=0; i<n; ++i) {
      for (j=0; j<H[i]; ++j) A[s++][t] = '|'; /* Upward movement */
      for (j=0; j<W[i]; ++j) A[s][t++] = '-'; /* Rightward movement */
   }
   for (i=toth; i>=0; --i) {
      printf("%s\n", A[i]);
      free(A[i]);
   }
   for (i=0; i<=totw; ++i) printf("-");
   printf("\n");
   free(A);
}

int main ( int argc, char *argv[] )
{
   int n, m, i;
   int *W, *H, *W1, *H1;
   int **M, **T, **S;

   if (argc >= 3) { n = atoi(argv[1]); m = atoi(argv[2]); }
   else { n = NDFT; m = MDFT; }
   srand((unsigned int)time(NULL));

   /* Generate steps */
   W = (int *)malloc(n * sizeof(int));
   H = (int *)malloc(n * sizeof(int));
   initSteps(n,W,H);

   /* Compute merging costs */
   M = (int **)malloc(n * sizeof(int *));
   for (i=0; i<n; ++i) M[i] = (int *)malloc(n * sizeof(int));

   /* Compute optimal costs along with the solution */
   calcMergeCosts(n,W,H,M);
   T = (int **)malloc(n * sizeof(int *));
   S = (int **)malloc(n * sizeof(int *));
   for (i=0; i<n; ++i) {
      T[i] = (int *)malloc(m * sizeof(int));
      S[i] = (int *)malloc(m * sizeof(int));
   }
   calcOptimalSoln(n,m,M,T,S);
   printf("Minimum restructuring cost = %d\n", T[n-1][m-1]);

   /* Generate the sequence of new steps */
   W1 = (int *)malloc(m * sizeof(int));
   H1 = (int *)malloc(m * sizeof(int));
   calcNewSteps(n,m,W,H,S,W1,H1);

   /* Print input and output */
   printf("+++ Old steps:\n");
   drawSteps(n,W,H);
   printf("+++ New steps:\n");
   drawSteps(m,W1,H1);

   /* Free allocated memrory */
   free(W); free(H); free(W1); free(H1);
   for (i=0; i<n; ++i) free(M[i]);
   for (i=0; i<m; ++i) { free(T[i]); free(S[i]); }
   free(M); free(T); free(S);

   exit(0);
}
