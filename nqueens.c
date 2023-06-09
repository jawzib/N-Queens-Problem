#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

//Define macros
#define MAX_STEPS 1000000
#define INF 1000000

//Define DIM macro for board size
#define DIM 1000

void review_previous_conflicts(int queen, int dim, int queens[DIM], int row_conflicts[DIM], int main_diag_conflicts[DIM * 2], int secondary_diag_conflicts[DIM * 2])
{
    //Initialize variables
    int i;
    //Set min conflicts to large number
    int min_conflicts = INF;
    int row_count = 0;
    int new_row;
    int conflicts;
    //Array to store the potential rows for queen to go in it's column
    static int potential_rows[DIM];
    //Loop over all rows in queen's column
    for (i = 0; i < dim; i++)
    {
        //Calculate the number of conflicts for each row in the column
        conflicts = 0;
        conflicts += row_conflicts[i];
        conflicts += main_diag_conflicts[dim - i + queen];
        conflicts += secondary_diag_conflicts[i + queen];
        //If the current cell's conflicts are less than min conflicts update min_conflicts and potential_rows array
        if (conflicts < min_conflicts)
        {
            min_conflicts = conflicts;
            potential_rows[0] = i;
            row_count = 1;
        }
        //If the conflicts are equal to the min_conflicts add the row to the potential_rows array
        else if (conflicts == min_conflicts)
        {
            potential_rows[row_count] = i;
            row_count++;
        }
    }
    //Get new row randomly
    new_row = potential_rows[rand() % row_count];
    //Increment frequency arrays for the new queen
    row_conflicts[new_row]++;
    main_diag_conflicts[dim - new_row + queen]++;
    secondary_diag_conflicts[queen + new_row]++;
    queens[queen] = new_row;
}
int check_previous_conflicts(int col, int dim, int queens[DIM])
{
    static int column_conflicts[DIM];
    memset(column_conflicts, 0, sizeof(column_conflicts));
    int i;
    int diagnol1, diagnol2;
    for (i = 0; i < col; i++)
    {
        column_conflicts[queens[i]]++;
        diagnol1 = queens[i] - i + col;
        diagnol2 = queens[i] + i - col;
        if (diagnol1 >= 0 && diagnol1 < dim)
        {
            column_conflicts[diagnol1]++;
        }
        if (diagnol2 >= 0 && diagnol2 < dim)
        {
            column_conflicts[diagnol2]++;
        }
    }
    int minimum = INF;
    static int potential_rows[DIM];
    int row_count = 0;
    for (i = 0; i < dim; i++)
    {
        if (column_conflicts[i] < minimum)
        {
            minimum = column_conflicts[i];
            row_count = 1;
            potential_rows[0] = i;
        }
        else if (column_conflicts[i] == minimum)
        {
            potential_rows[row_count] = i;
            row_count++;
        }
    }
    return potential_rows[rand() % row_count];
}

void place_queens(int dim, int queens[DIM], int row_conflicts[DIM], int main_diag_conflicts[DIM * 2], int secondary_diag_conflicts[DIM * 2])
{
    //Loop through all the columns of the board
    int i;
    for (i = 0; i < DIM; i++)
    {
        //Call review_previous_conflicts to get all the other conflicts from previously placed queens to decide where to place current (ith queen)
        review_previous_conflicts(i, dim, queens, row_conflicts, main_diag_conflicts, secondary_diag_conflicts);
        if (i % (dim / 10) == 0)
        {
            printf("Queen %d placed\n", i);
        }
    }
}

void min_conflicts(int queen, int dim, int queens[dim], int row_conflicts[dim], int main_diag_conflicts[dim * 2], int secondary_diag_conflicts[dim * 2])
{
    //Initialize variables
    //Set min_conflicts to large number
    int min_conflicts = INF;
    static int potential_rows[DIM];
    int i;
    int conflicts;
    int row_count = 1;
    int new_row;
    //Loop through all rows in queen's column
    for (i = 0; i < dim; i++)
    {
        //Calculate the number of conflicts for each row
        conflicts = 0;
        conflicts += row_conflicts[i];
        conflicts += main_diag_conflicts[dim - i + queen];
        conflicts += secondary_diag_conflicts[i + queen];
        //If queen is the currently observerd row decrement the conflicts by 3 (Want to calculate as if queen is not there)
        if (i == queens[queen])
        {
            conflicts -= 3;
        }
        //If conflicts is less than min_conflicts then update min_conflicts
        if (conflicts < min_conflicts)
        {
            min_conflicts = conflicts;
            potential_rows[0] = i;
            row_count = 1;
        }
        //If the current conflicts equals the min_conflicts then add the row to a potential_rows array
        else if (conflicts == min_conflicts)
        {
            potential_rows[row_count] = i;
            row_count++;
        }
    }
    //Get new row randomly from potential_rows array
    new_row = potential_rows[rand() % row_count];
    //Reduce the frequencies of previously placed queen by 1
    row_conflicts[queens[queen]]--;
    main_diag_conflicts[dim - queens[queen] + queen]--;
    secondary_diag_conflicts[queens[queen] + queen]--;
    //Increment the frequencies of newly placed queen by 1
    row_conflicts[new_row]++;
    main_diag_conflicts[dim - new_row + queen]++;
    secondary_diag_conflicts[new_row + queen]++;
    //Set new queen row
    queens[queen] = new_row;
}

int get_queen_conflicts(int dim, int queen_conflicts[dim], int queens[DIM], int row_conflicts[dim], int main_diag_conflicts[dim * 2], int secondary_diag_conflicts[dim * 2])
{
    //Initialize queen_conflicts array to 0
    memset(queen_conflicts, 0, sizeof(int) * DIM);
    //Initialize variables
    int i;
    int total_conflicts = 0;
    int queen;
    //Loop over queens array to calculate queens conflicts
    for (i = 0; i < dim; i++)
    {
        //Calculate queen conflicts from conflict arrays
        queen = queens[i];
        queen_conflicts[i] += row_conflicts[queen] - 1;
        queen_conflicts[i] += main_diag_conflicts[dim - queen + i] - 1;
        queen_conflicts[i] += secondary_diag_conflicts[queen + i] - 1;
        //Increment total conflicts variable
        total_conflicts += queen_conflicts[i];
    }
    return total_conflicts;
}

int solve(int dim, int queens[DIM], int max_steps, int row_conflicts[dim], int main_diag_conflicts[dim * 2], int secondary_diag_conflicts[dim * 2])
{
    //Initialize variables
    int moves = 0;
    int previous_queen = -1;
    static int queen_conflicts[DIM];
    static int potential_queens[DIM];
    int potential_queen_count;
    int i;
    int total_conflicts;
    int queen;
    int j;
    for (i = 0; i < max_steps; i++)
    {
        //Calculate total conflicts by getting all the queens conflicts
        total_conflicts = get_queen_conflicts(dim, queen_conflicts, queens, row_conflicts, main_diag_conflicts, secondary_diag_conflicts);
        //If there are no conflicts then return the number of moves taken
        if (total_conflicts == 0)
        {
            return moves;
        }
        //Loop through all queens conflicts to find new queen to reassign
        potential_queen_count = 0;
        for (j = 0; j < dim; j++)
        {
            //If the queen wasn't the previously moved queens and has more than 0 conflicts add to potential queens array
            if (queen_conflicts[j] > 0 && j != previous_queen)
            {
                potential_queens[potential_queen_count] = j;
                potential_queen_count++;
            }
        }
        //Randomly choose queen from potential queens array
        queen = potential_queens[rand() % potential_queen_count];
        previous_queen = queen;
        //Call min_conflicts function to assign the queen
        min_conflicts(queen, dim, queens, row_conflicts, main_diag_conflicts, secondary_diag_conflicts);
        //Increment the number of moves
        moves++;
    }
    //MAX_STEPS exhausted return -1
    printf("No solution found in %d iterations\n", moves);
    return -1;
}

//Function to read in board
void read_board(char *filename, int dim, int queens[DIM])
{
    FILE *fp;
    fp = fopen(filename, "r");
    //Numbers are spaced separated on first line of file
    for (int i = 0; i < dim; i++)
    {
        fscanf(fp, "%d ", &queens[i]);
    }
    fclose(fp);
}

//Function to create the conflict arrays if the board was read in from file
void create_conflict_arrays(int dim, int queens[DIM], int row_conflicts[dim], int main_diagnol_conflicts[dim * 2], int secondary_diagnol_conflicts[dim * 2])
{

    for (int i = 0; i < dim; i++)
    {
        row_conflicts[queens[i]]++;
        main_diagnol_conflicts[dim - queens[i] + i]++;
        secondary_diagnol_conflicts[queens[i] + i]++;
    }
}

//Gets total number of conflicts on the board
int get_conflicts(int dim, int queens[DIM], int row_conflicts[dim], int main_diag_conflicts[dim * 2], int secondary_diag_conflicts[dim * 2])
{
    int i;
    int conflicts = 0;
    for (i = 0; i < dim * 2; i++)
    {
        //Detects cycles in graphical representation with equation (N(N-1))/1
        if (i < dim)
        {
            conflicts += (row_conflicts[i] * (row_conflicts[i] - 1)) / 2;
        }
        conflicts += (main_diag_conflicts[i] * (main_diag_conflicts[i] - 1)) / 2;
        conflicts += (secondary_diag_conflicts[i] * (secondary_diag_conflicts[i] - 1)) / 2;
    }
    return conflicts;
}

//Print board to console
void print_board(int dim, int queens[DIM])
{
    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            if (queens[j] == i)
            {
                printf("Q ");
            }
            else
            {
                printf(". ");
            }
        }
        printf("\n");
    }
}

//Write board to file
void write_board(char *filename, int dim, int queens[DIM])
{
    FILE *fp;
    fp = fopen(filename, "w");
    for (int i = 0; i < dim; i++)
    {
        fprintf(fp, "%d ", queens[i]);
    }
    fclose(fp);
}
/*
Compilation Instructions
Set DIM to desired board size in the .c file
Compile Command:
gcc nqueens_optimized.c -o nqueens -Ofast
Run Commands:
Create a new board and store the initial placements and results to a file with specificied title of dimension, date and time of run
./nqueens -c
Read in board from file and creates a solution file with specified title of dimension, date and time of run
./nqueens -r <filename>.txt
*/
int main(int argc, char *argv[])
{
    printf("Starting program\n");
    //Seed random number generator
    srand(time(NULL));
    //Initialize variables
    clock_t start, end;
    start = clock();
    double time_taken;
    int N = DIM;
    static int queens[DIM];
    static int row_conflicts[DIM];
    static int main_diag_conflicts[DIM * 2];
    static int secondary_diag_conflicts[DIM * 2];
    //Initialize conflict arrays
    memset(row_conflicts, 0, sizeof(int) * DIM);
    memset(main_diag_conflicts, 0, sizeof(int) * DIM * 2);
    memset(secondary_diag_conflicts, 0, sizeof(int) * DIM * 2);

    //If user wants new board created place the queens using the min_conflicts heuristic
    if (strcmp(argv[1], "-c") == 0)
    {
        queens[0] = rand() % N;
        printf("Placing Queens\n");
        place_queens(N, queens, row_conflicts, main_diag_conflicts, secondary_diag_conflicts);
        end = clock();
        time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("Time taken to place queens: %f seconds\n", time_taken);
    }
    //Otherwise read in the board and create the conflict array
    else
    {
        printf("Reading board from file %s\n", argv[2]);
        read_board(argv[2], N, queens);
        create_conflict_arrays(N, queens, row_conflicts, main_diag_conflicts, secondary_diag_conflicts);
    }
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char queen_filename[100];
    //Write the initial placements to file if -c was specified
    if (strcmp(argv[1], "-c") == 0)
    {
        snprintf(queen_filename, sizeof(queen_filename), "%d_queens_initial_%d-%02d-%02d-%02d-%02d-%02d.txt", N, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        write_board(queen_filename, N, queens);
        printf("Initial placements written to %s\n", queen_filename);
    }

    //Get total conflicts
    int total_conflicts = get_conflicts(N, queens, row_conflicts, main_diag_conflicts, secondary_diag_conflicts);
    printf("Total Conflicts: %d\n", total_conflicts);
    printf("\n");
    if (total_conflicts == 0)
    {
        printf("Board is already solved\n");
        return 0;
    }
    //Solve the board
    printf("Solving Board\n");
    start = clock();
    int moves = solve(N, queens, MAX_STEPS, row_conflicts, main_diag_conflicts, secondary_diag_conflicts);
    end = clock();
    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Board solved in %d moves\n", moves);
    printf("Time taken to solve board: %f seconds\n", time_taken);


    //Write solution to file
    char final_queen_filename[100];
    snprintf(final_queen_filename, sizeof(final_queen_filename), "%d_queens_solution_%d-%02d-%02d-%02d-%02d-%02d.txt", N, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    write_board(final_queen_filename, N, queens);
    printf("Solution written to %s\n", final_queen_filename);

    return 1;
}