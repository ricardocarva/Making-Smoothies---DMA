/* COP 3502C Assignment 1
This program is written by: Ricardo Ferreira Carvalheira */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leak_detector_c.h"
#include <stdbool.h>


#define MAXSTRLENGTH 20
FILE *inFile, *outFile;

typedef struct item { //This stores one component of a smoothie recipe
int itemID; //The itemID represents the ingredient number
int numParts; // numParts represents the number of parts of that ingredient
} item;

typedef struct recipe { //This stores one smoothie recipe
int numItems; //numItems stores the number of different ingredients
item* itemList; //itemList will be a dynamically allocated array of item where each slot of the array stores one ingredient from the recipe
int totalParts; //and totalParts will equal the sum of the numParts of each ingredient in the smoothie
} recipe;

char** readIngredients(int *numIngredients); //reads all ingredients
recipe* readRecipe(); //reads single recipe
recipe** readAllRecipes(int *numRecipes); //reads all recipes
double* calculateOrder(int ingredientCount, int numSmoothies, recipe** recipeList); //calculates orders
void printOrder(char** ingredientNames, double* orderList, int numIngredients);

void freeIngredients(char** ingredientsList, int numIngredients); // all the memory pointed to by ingredientList is freed.
void freeRecipes(recipe** allRecipes, int numRecipes);//all the memory pointed to by allRecipes is freed

bool checkIngredientsID(int numIngredients); //Function to check if the ID is valid

bool checkNumberOfRecipes(int numRecipes); //Function to check if the ID is valid. 


int main(void) {
  atexit(report_mem_leak);
  char** ingredientsList; //Pointer to pointer to receive the ingredients list
  int numIngredients; //Variable to store the number of ingredients
  int numRecipes; //stores the number of recipes
  int numStores, numSmoothies;
  bool setNumIngredients;
  bool setNumRecipes;

  recipe** allRecipes; // recipe pointer that points to all recipes
  double* orderList; //pointer to the order list


	inFile = fopen("in.txt", "r"); //opens the file name
 	outFile = fopen("out.txt", "w"); //opens the file name

  if (inFile!= NULL){
  ingredientsList = readIngredients(&numIngredients);   //readIngredients will store the ingredients.
  if(ingredientsList==NULL) //If it does not meet the number of ingredients condition, it returns NULL
    {
      fclose(inFile); //Closes the files
      fclose(outFile); //Closes the files
      exit(1);
    }
  setNumIngredients = checkIngredientsID(numIngredients); //Set the number of ingredients to be used later to check if the ingredients ID are valid
  allRecipes = readAllRecipes (&numRecipes); //readAllRecipes will store the recipes.
  if(allRecipes==NULL) //If it does not meet the numSmoothies condition, it returns NULL
  {
    freeIngredients(ingredientsList, numIngredients); //Then, frees the memory allocated for ingredients
    fclose(inFile); //Closes the files
    fclose(outFile); //Closes the files
    exit(1);
  }
  setNumRecipes=(checkNumberOfRecipes(numRecipes)); //set number of recipes

  fscanf(inFile,"%d", &numStores); //scans from the input file the first digit and stores in  num digit
    if(numStores<1||numStores>100)
    {
      printf("Number of stores %d is not valid: Interval is (1 ≤ k ≤ 100)\n", numStores);
      fprintf(outFile, "Number of stores %d is not valid: Interval is (1 ≤ k ≤ 100)", numStores);
      fprintf(outFile, "\n");
      freeIngredients(ingredientsList, numIngredients); //free memory
      freeRecipes(allRecipes, numRecipes); //free memory
      fclose(inFile); //Closes the files    
      fclose(outFile); //Closes the files
      exit(1);
    }
  for(int i=0; i<numStores;i++)
  {
    fscanf(inFile,"%d", &numSmoothies); //scans from the input file the first digit and stores in  num digit
    if(numSmoothies<1||numSmoothies>numRecipes)//Check if (1 ≤ r ≤ s)
    {
      printf("Number of different smoothies the store# %d offers is not valid: Interval is (1 ≤ r ≤ s)\n", i+1);
      fprintf(outFile, "Number of different smoothies the store# %d offers is not valid: Interval is (1 ≤ r ≤ s)", i+1);
      fprintf(outFile, "\n");
      freeIngredients(ingredientsList, numIngredients); //free memory
      freeRecipes(allRecipes, numRecipes); //free memory
      fclose(inFile); //Closes the files    
      fclose(outFile); //Closes the files
      exit(1);
    }
    
    printf("Store #%d:\n",i+1);
    fprintf(outFile, "Store #%d:\n",i+1);

 
    orderList = calculateOrder( numIngredients, numSmoothies, allRecipes);
    if(orderList==NULL) //If it does not meet the numSmoothies condition, it returns NULL
      {
        freeIngredients(ingredientsList, numIngredients); //Then, frees the memory allocated for ingredients
        freeRecipes(allRecipes, numRecipes);
        
        fclose(inFile); //Closes the files
        fclose(outFile); //Closes the files
        exit(1);
      }
    printOrder(ingredientsList, orderList, numIngredients);

  }
 
  }
 
  freeIngredients(ingredientsList, numIngredients);
  freeRecipes(allRecipes, numRecipes);


  fclose(inFile);
  fclose(outFile);

  return 0;
}

char** readIngredients(int *numIngredients)
{
  char** ingredientsArray;
  char ingredient[MAXSTRLENGTH]; //The ingredients should not have more than 19 characters + null
  int i, ingredientWordLength;
  
  fscanf(inFile,"%d", numIngredients); //scans from the input file the first digit and stores in  num digit
  if((*numIngredients>=100000) || (*numIngredients<0)) //pre-condition: The first line will contain a single positive integer, n (n ≤ 105), representing the number of possible smoothie ingredients.
  {
    printf("Number of possible smoothie ingredients is not valid. It should be a single positive integer less than or equal to 100000.\n");
    fprintf(outFile, "Number of possible smoothie ingredients is not valid. It should be a single positive integer less than or equal to 100000.");
    fprintf(outFile, "\n");
    return NULL; //returns NULL if it does not meet the pre-condition
  }

  
  ingredientsArray = (char**)malloc(*numIngredients*sizeof(char*)); //allocates memory for arrays of pointers to strings

    for(i=0;i<*numIngredients;i++)
    {
      fscanf(inFile,"%s",ingredient);  //scan each ingredients
      if(strlen(ingredient)>(20))
      {
        printf("Ingredient name should have maximum of 20 characters.");
        return NULL;
      }
      ingredientWordLength=strlen(ingredient)+1; //get the length of the ingredient and adds + for the null character
      ingredientsArray[i]=(char*)malloc(ingredientWordLength*sizeof(char)); //allocates the  memory for the string in the ingredientsArray
      strcpy(ingredientsArray[i], ingredient); //copies the string from ingredient to the array

    }

  return ingredientsArray;
}

void freeIngredients(char** ingredientsList, int numIngredients)
{
  for(int i=0; i<numIngredients;i++)
    free(ingredientsList[i]);
  free(ingredientsList);
  return;
}

void freeRecipes(recipe** recipeList, int numRecipes)
{

  for(int i=0; i<numRecipes;i++)
  {
   free(recipeList[i]->itemList);
    free(recipeList[i]);
  }
   free(recipeList);
  return;

}


recipe** readAllRecipes(int *numRecipes)
{
  recipe** smoothieList; 

  fscanf(inFile,"%d", numRecipes); //scans from the input file the first of the recipe line and stores in  numRecipes
  if((*numRecipes>=100000) || (*numRecipes<0)) //pre-condition
  {
    printf("Number of Smoothies is not valid. It should be larger than 0 and less than or equal to 100000.\n");
    fprintf(outFile, "Number of Smoothies is not valid. It should be larger than 0 and less than or equal to 100000.");
    fprintf(outFile, "\n");
    return NULL; //returns NULL if it does not meet the pre-condition
  }
  smoothieList = (recipe**) malloc (*numRecipes*sizeof(recipe*));//allocates memory for the array that is pointing to the list of all smoothie recipes

  for(int i=0; i<*numRecipes;i++){
    smoothieList[i]=readRecipe();
      if(smoothieList[i]==NULL) //If the number of smoothie ingredients is not valid, it returns NULL
      {
        free(smoothieList); //Free the memory and returns to Main
        return NULL;
      }
  }
  return smoothieList;
}


recipe* readRecipe()
{
  recipe* eachRecipe;
  int totalItems; //stores the total of items of a recipe
  int sumOfParts=0; //stores the total of parts of a recipe

  fscanf(inFile,"%d", &totalItems); //Reads in details of a recipe such as numItems. However, we stored it in totalItems to pass it later to the struct

  if((totalItems>=100) || (totalItems<=0)) //pre-condition
  {
    printf("Number of different Smoothies ingredients is not valid. It should be larger than 1 and less than or equal to 100.\n");
    fprintf(outFile, "Number of different Smoothies ingredients is not valid. It should be larger than 1 and less than or equal to 100");
    fprintf(outFile, "\n");
    return NULL; //returns NULL if it does not meet the pre-condition
  }

  eachRecipe = (recipe *) malloc (sizeof(recipe)); //Dynamically allocates space for a single recipe
  
  eachRecipe->itemList= (item*) malloc (totalItems*sizeof(item)); //Dynamically allocates an array of item of the proper size

  eachRecipe->numItems=totalItems; //updates the numItems field of the struct
  
  for(int i=0;i<eachRecipe->numItems;i++)
  {
      fscanf(inFile,"%d", &eachRecipe->itemList[i].itemID); //scans from the input file the first of the recipe line and stores in  numRecipes
        if(checkIngredientsID(eachRecipe->itemList[i].itemID)) //If the ingredient number is not between (0 ≤ I1 < n), we print, free the memory and return Null
        {
          printf("IngredientID %d is not valid. It should be larger than 0 and less than or equal to the number of ingredients.\n",eachRecipe->itemList[i].itemID);
          fprintf(outFile, "IngredientID %d is not valid. It should be larger than 0 and less than or equal to the number of ingredients.",eachRecipe->itemList[i].itemID);
          fprintf(outFile, "\n");
          free(eachRecipe->itemList);
          free(eachRecipe);
          return NULL;
        }
      fscanf(inFile,"%d", &eachRecipe->itemList[i].numParts); //scans from the input file the first of the recipe line and stores in  numRecipes
        if(eachRecipe->itemList[i].numParts<1||eachRecipe->itemList[i].numParts>1000) //If the ingredient number is not between (0 ≤ I1 < n),    we print, free the memory and return Null
        {
          printf("Ingredient Parts interval is not valid. It should be (1 ≤ R1 ≤ 1000) where R1 represents the number of parts.\n");
          fprintf(outFile, "Ingredient Parts interval is not valid. It should be (1 ≤ R1 ≤ 1000) where R1 represents the number of parts.");
          fprintf(outFile, "\n");
          free(eachRecipe->itemList);
          free(eachRecipe);
          return NULL;
        }
      sumOfParts += eachRecipe->itemList[i].numParts;
  }
  eachRecipe->totalParts=sumOfParts; 

  return eachRecipe;
}


double* calculateOrder(int ingredientCount, int numSmoothies, recipe** recipeList)
{
  double* amtOfEachItem;
  
  amtOfEachItem = (double*) calloc ( ingredientCount, sizeof(double));

  for(int i=0; i<numSmoothies;i++)  
  {
    int recipeType, poundsOfIngredients;
    fscanf(inFile,"%d", &recipeType); //Reads in details of a recipe such as numItems. However, we stored it in totalItems to pass it later to the struct
    
    if(recipeType<0||checkNumberOfRecipes(recipeType)) //If the recipe number is valid (0 ≤ S1 < s), we print, free the memory and return Null
        {
          printf("Recipe %d is not valid. It should be larger  or equal than 0 and less than or equal to the number of Smoothie Recipes.\n", recipeType);
          fprintf(outFile, "Recipe %d is not valid. It should be larger  or equal than 0 and less than or equal to the number of Smoothie Recipes.", recipeType);
          fprintf(outFile, "\n");
          free(amtOfEachItem); //free the memory
          return NULL;
        }
    fscanf(inFile,"%d", &poundsOfIngredients); //Reads in details of a recipe such as poundsOfIngredients. 
          if(poundsOfIngredients<1||poundsOfIngredients>1000) //If the ingredient number is not between (0 ≤ I1 < n), we print, free the memory and return Null
        {
          printf("Weight Sold %d is not valid. It should be larger  or equal than 0 and less than or equal to 1000 in pounds.\n", poundsOfIngredients);
          fprintf(outFile, "Weight Sold %d is not valid. It should be larger  or equal than 0 and less than or equal to 1000 in pounds.", poundsOfIngredients);
          fprintf(outFile, "\n");
          free(amtOfEachItem); //free the memory
          return NULL;
        }
    for(int j=0;j<recipeList[recipeType]->numItems;j++){ //loop through recipeList in that specific recipe
      
      int itemIDIndex=recipeList[recipeType]->itemList[j].itemID; //The itemIDIndex will store the value of the itemID from the recipe
      
        if(recipeList[recipeType]->itemList[j].numParts!=0)//We don't want to divide by zero
        {              
              amtOfEachItem[itemIDIndex]+=(double) (recipeList[recipeType]->itemList[j].numParts*poundsOfIngredients)/(recipeList[recipeType]->totalParts); //update the amtOfEachItem index
        }
      }
  }
  
  return amtOfEachItem; //return double array
}

void printOrder(char** ingredientNames, double* orderList, int numIngredients)
{
  for(int i=0; i<numIngredients;i++)
  {
    if(orderList[i]!=0)
    {
    printf("%s %f\n", ingredientNames[i], orderList[i]);
    fprintf(outFile, "%s %f\n", ingredientNames[i], orderList[i]);
    }
  }     
  free(orderList); //free double array right AFTER the calculation of how much of each ingredient needs to be ordered completes and is printed to the screen
  printf("\n");
  fprintf(outFile, "\n");
  return;
}

bool checkIngredientsID(int numIngredients)
{ //Function to check if the ID is valid
  static bool firstOccurenceFlag=true; 
  static int numberOfIngredients=0; //value won't be initialized again in multiple calls

  if(firstOccurenceFlag) //If this is the first occurence, we set the numberOfIngredients=numIngredients. What we are really doing is setting "n"
  { 
    firstOccurenceFlag = false;
  numberOfIngredients=numIngredients;
   }
  else
  { //If it's not the first occurence, we will compare the current number to see if it's ID is valid. If it's greated or equal, return true. (I1 >= n)
    if(numIngredients>=numberOfIngredients||numIngredients<0)
    {
      return true; //This way we can enter the if in the other function to exit the code
    }
  }
  return false; 
}



bool checkNumberOfRecipes(int numRecipes)
{ //Function to check if the recipe is valid
  static bool firstOccurenceFlag=true; 
  static int maxNumRecipes=0; //value won't be initialized again in multiple calls

  if(firstOccurenceFlag) //If this is the first occurence, we set the maxNumRecipes=numRecipes. What we are really doing is setting "s"
  { 
    firstOccurenceFlag = false;
    maxNumRecipes=numRecipes;
   }

   else
  { //If it's not the first occurence, we will compare the current number to see if its recipe is valid. If it's greated or equal, return true. (s >= S1)
    if((numRecipes>=maxNumRecipes))
    {
      return true; //This way we can enter the if in the other function to exit the code
    }
  }
  return false; 

}