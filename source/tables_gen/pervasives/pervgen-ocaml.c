/*****************************************************************************/
/* File pervgen-ocaml.c. This files contains function definitions for        */
/* generating files pervasive.mli and pervasive.ml.                          */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pervgen-ocaml.h"
#include "ocamlcode.h"


static char* addLine(char* str, char* addOn)
{
    int length = (str ? strlen(str) : 0) + strlen(addOn) + 2;
    char* newStr = UTIL_mallocStr(length);
    
    if (str) {
        strcpy(newStr, str);
        strcat(newStr, addOn);
    } else strcpy(newStr, addOn);
    strcat(newStr, "\n\n");    
    return newStr;
}

static char* addStr(char* str, char* addOn)
{
    int length = (str ? strlen(str) : 0) + strlen(addOn);
    char* newStr = UTIL_mallocStr(length);
    
    if (str) {
        strcpy(newStr, str);
        strcat(newStr, addOn);
    } else strcpy(newStr, addOn);
    return newStr;
}

/**************************************************************************/
/* generating pervasive kind relevant part                                */
/**************************************************************************/
static char* kindVarList = NULL;           //kind variable definitions
static char* buildPervKindBody = NULL;     //buildPervKind function defs
static char* kindVarDecs = NULL;           //kind vars in signature
static char* isKindFuncDecs = NULL;        //is kind function decs
static char* isKindFuncDefs = NULL;        //is kind function defs
void ocamlGenKind(char* kindName, char* kVarName, char* arity, char* offset)
{ 
    char* kindVarName  = OC_mkKVarName(kVarName);
    char* funcName     = OC_mkIsKindFuncName(kindVarName);
    char* kindVar      = OC_mkKindVar(kindVarName, kindName, arity, offset); 
    char* kindTabEntry = OC_mkTabEntry(kindName, kindVarName);
    char* kindVarDec   = OC_mkKindVarDec(kindVarName);
    char* funcDec      = OC_mkIsKindFuncDec(funcName);
    char* funcDef      = OC_mkIsKindFuncDef(funcName, kindVarName);
    char *myKindVarList, *myBuildPervKindBody, *myKindVarDecs, 
        *myisKindFuncDecs, *myisKindFuncDefs;
 
    free(kindVarName);
    
    myKindVarList = addLine(kindVarList, kindVar);
    free(kindVarList); free(kindVar);
    kindVarList = myKindVarList;
    
    myBuildPervKindBody = addStr(buildPervKindBody, kindTabEntry);
    free(buildPervKindBody); free(kindTabEntry);
    buildPervKindBody = myBuildPervKindBody;

    myKindVarDecs = addStr(kindVarDecs, kindVarDec);
    free(kindVarDecs); free(kindVarDec);
    kindVarDecs = myKindVarDecs;
    
    myisKindFuncDecs = addStr(isKindFuncDecs, funcDec);
    free(isKindFuncDecs); free(funcDec);
    isKindFuncDecs = myisKindFuncDecs;
    
    myisKindFuncDefs = addLine(isKindFuncDefs, funcDef);
    free(isKindFuncDefs); free(funcDef);
    isKindFuncDefs = myisKindFuncDefs;
}

static char* kindML = NULL;            //kind relevant code in pervasive.ml
static char* kindMLI = NULL;           //kind relevant code in pervasive.mli 

void ocamlGenKinds()
{
    char* buildTabFunc = OC_mkBuildKTabFunc(buildPervKindBody);
    int   length = strlen(kindVarList) + strlen(buildTabFunc) +  
        strlen(isKindFuncDefs) + 1;
    
    kindML = UTIL_mallocStr(length + 1);
    strcpy(kindML, kindVarList);
    strcat(kindML, "\n");
    strcat(kindML, buildTabFunc);
    strcat(kindML, "\n");
    strcat(kindML, isKindFuncDefs);
    
    free(buildPervKindBody); free(buildTabFunc); free(kindVarList);
    free(isKindFuncDefs);
    
    length = strlen(kindVarDecs) + strlen(isKindFuncDecs) + 2;
    kindMLI = UTIL_mallocStr(length);
    strcpy(kindMLI, kindVarDecs);
    strcat(kindMLI, "\n\n");
    strcat(kindMLI, isKindFuncDecs);
    free(kindVarDecs);  free(isKindFuncDecs);
}

/**************************************************************************/
/* generating pervasive type skeleton relevant part                       */
/**************************************************************************/
static char* tySkelVarList = NULL;        //type skel vars

void ocamlGenTySkel(char* ind, Type tySkel)
{
    char* varName = OC_mkTySkelVarName(ind);
    char* tySkelText = OC_genTySkel(tySkel);
    char* tySkelVarDef = OC_mkTYSkelVar(varName, tySkelText);
    int   length = (tySkelVarList ? strlen(tySkelVarList) : 0) + 
        strlen(tySkelVarDef) + 1;
    char* mytySkelVarList = UTIL_mallocStr(length + 1);
    
    free(varName); free(tySkelText);
    
    mytySkelVarList = addLine(tySkelVarList, tySkelVarDef);
    free(tySkelVarList); free(tySkelVarDef);
    tySkelVarList = mytySkelVarList;
}

/**************************************************************************/
/* generating pervasive constants relevant part                           */
/**************************************************************************/
static char* constVarList = NULL;         //constant vars
static char* buildPervConstBody = NULL;   //buildPervConst function defs
static char* constVarDecs = NULL;         //constant vars in signature
static char* isConstFuncDecs = NULL;      //is constant function decs
static char* isConstFuncDefs = NULL;      //is constant function defs

void ocamlGenConst(char* ind, char* name, char* cVarName,  OP_Fixity fixity, 
                   OP_Prec prec, UTIL_Bool tyPrev, UTIL_Bool redef, int tesize,
                   int tyskelInd, int neededness, OP_Code codeInfo, 
                   char* offset)
{
    char* constVarName = OC_mkCVarName(cVarName);
    char* funcName     = OC_mkIsConstFuncName(constVarName);
    char* tyskelText   = UTIL_itoa(tyskelInd);
    char* tyskelName   = OC_mkTySkelVarName(tyskelText);
    
    char* constVar     = OC_mkConstVar(name, fixity, prec, tyPrev, tyskelName,
                                       tesize, neededness, codeInfo, redef,
                                       constVarName, offset);
    char* constTabEntry = OC_mkTabEntry(name, constVarName);
    char* constVarDec   = OC_mkConstVarDec(constVarName);
    char* funcDec       = OC_mkIsConstFuncDec(funcName);
    char* funcDef       = OC_mkIsConstFuncDef(funcName, constVarName);
    
    char *myConstVarList, *myBuildPervConstBody, *myConstVarDecs,
        *myisConstFuncDecs, *myisConstFuncDefs;
    
    free(constVarName);  free(funcName); free(tyskelName); free(tyskelText);

    myConstVarList = addLine(constVarList, constVar);
    free(constVarList);  free(constVar);
    constVarList = myConstVarList;
    
    myBuildPervConstBody = addStr(buildPervConstBody, constTabEntry);
    free(buildPervConstBody); free(constTabEntry);
    buildPervConstBody = myBuildPervConstBody;
    
    myConstVarDecs = addStr(constVarDecs, constVarDec);
    free(constVarDecs); free(constVarDec);
    constVarDecs = myConstVarDecs;
    
    myisConstFuncDecs = addStr(isConstFuncDecs, funcDec);
    free(isConstFuncDecs); free(funcDec);
    isConstFuncDecs = myisConstFuncDecs;
    
    myisConstFuncDefs = addLine(isConstFuncDefs, funcDef);
    free(isConstFuncDefs); free(funcDef);
    isConstFuncDefs = myisConstFuncDefs;
}

static char* constMLI = NULL;            //const relevant code in pervasive.mli
static char* constML = NULL;             //const relevant code in pervasive.ml

void ocamlGenConsts()
{
    char* tyskels = OC_mkFixedTySkels(tySkelVarList);
    char* varDefs = OC_mkGenericConstVar(constVarList);
    char* varDecs = OC_mkGenericConstVarDec(constVarDecs);
    char* buildFuncBody = OC_mkGenericConstTabEntry(buildPervConstBody);
    char* buildTabFunc = OC_mkBuildCTabFunc(buildFuncBody);
    char* funcDefs = OC_mkGenericConstFuncDefs(isConstFuncDefs);
    char* funcDecs = OC_mkGenericConstFuncDecs(isConstFuncDecs);
    

    int   length = strlen(varDefs) + strlen(buildTabFunc) + strlen(funcDefs) 
        + 2;
    
    tySkelVarList = tyskels;
    
    constML = UTIL_mallocStr(length);
    strcpy(constML, varDefs);          free(varDefs);
    strcat(constML, "\n");
    strcat(constML, buildTabFunc);     free(buildTabFunc); free(buildFuncBody);
    strcat(constML, "\n");    
    strcat(constML, funcDefs);         free(funcDefs);

    length = strlen(varDecs) + strlen(funcDecs) + 2;
    constMLI = UTIL_mallocStr(length);

    strcpy(constMLI, varDecs);    free(varDecs);
    strcat(constMLI, "\n\n");
    strcat(constMLI, funcDecs);   free(funcDecs);
}

static char* constProperty = NULL;
void ocamlCollectConsts(char* name, int last) 
{
    char* constName = OC_mkCVarName(name);
    char* cond      = OC_mkCompare(constName);
    char* body;

    free(constName);
    if (last) body = cond;
    else {
        if (constProperty) {
            body = OC_mkOr(cond, constProperty);
            free(constProperty);
            free(cond);
        } else body = cond;
    }
    constProperty = body;    
}

static char* regClob = NULL;
void ocamlGenRC()
{
    regClob = OC_mkRegClobFunc(constProperty);
    free(constProperty);
    constProperty = NULL;
}

static char* backTrack = NULL;
void ocamlGenBC()
{
    backTrack = OC_mkBackTrackFunc(constProperty);
    free(constProperty);
    constProperty = NULL;
}

/**************************************************************************/
/* generating fixed part of pervasive.ml and pervasive.mli                */
/**************************************************************************/
static char* fixedML = NULL;             //fixed part of pervasive.ml
static char* fixedMLI = NULL;            //fixed part of pervasive.mli

static void  ocamlGenFixedML()
{
    fixedML = OC_mkFixedML();
}
static void  ocamlGenFixedMLI()
{
    fixedMLI = OC_mkFixedMLI();
}

/***************************************************************************/
/* Dump code into pervasive.ml and pervasive.mli                           */
/***************************************************************************/
/* dump peravsive.ml   */
void spitOCPervasiveML()
{
    FILE* outFile;
    outFile = UTIL_fopenW("../../compiler/pervasive.ml");

    ocamlGenFixedML();
    fprintf(outFile, "%s\n\n", kindML);          free(kindML);
    fprintf(outFile, "%s\n\n", tySkelVarList);   free(tySkelVarList);
    fprintf(outFile, "%s\n\n", constML);         free(constML);
    fprintf(outFile, "%s\n\n", fixedML);         free(fixedML);
    fprintf(outFile, "%s\n\n", regClob);         free(regClob);
    fprintf(outFile, "%s\n\n", backTrack);       free(backTrack);
    
    

    UTIL_fclose(outFile);
}

/* dump peravsive.mli   */
void spitOCPervasiveMLI()
{
    FILE* outFile;
    outFile = UTIL_fopenW("../../compiler/pervasive.mli");
    
    ocamlGenFixedMLI();
    fprintf(outFile, "%s\n\n", kindMLI);         free(kindMLI);
    fprintf(outFile, "%s\n\n", constMLI);        free(constMLI);
    fprintf(outFile, "%s\n\n", fixedMLI);        free(fixedMLI);
    UTIL_fclose(outFile);
}

