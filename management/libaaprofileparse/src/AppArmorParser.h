/** \file
 *  This C header file was generated by $ANTLR version 3.0.1
 *
 *     -  From the grammar source file : AppArmor.g
 *     -                            On : 2007-08-19 11:55:22
 *     -                for the parser : AppArmorParserParser *
 * Editing it, at least manually, is not wise. 
 *
 * C language generator and runtime by Jim Idle, jimi|hereisanat|idle|dotgoeshere|ws.
 *
 * View this file with tabs set to 8 (:set ts=8 in gvim) and indent at 4 (:set sw=4 in gvim)
 *
 * The parser AppArmorParserhas the callable functions (rules) shown below,
 * which will invoke the code for the associated rule in the source grammar
 * assuming that the input stream is pointing to a token/text stream that could begin
 * this rule.
 * 
 * For instance if you call the first (topmost) rule in a parser grammar, you will
 * get the results of a full parse, but calling a rule half way through the grammar will
 * allow you to pass part of a full token stream to the parser, such as for syntax checking
 * in editors and so on.
 *
 * The parser entry points are called indirectly (by function pointer to function) via
 * a parser context typedef pAppArmorParser, which is returned from a call to AppArmorParserNew().
 *
 * The entry points for AppArmorParser are  as follows:
 *
 *  - ParseNode *      pAppArmorParser->startParse(pAppArmorParser)
 *  - void      pAppArmorParser->appArmorProfile(pAppArmorParser)
 *  - void      pAppArmorParser->newlineRule(pAppArmorParser)
 *  - void      pAppArmorParser->startOfLineComment(pAppArmorParser)
 *  - void      pAppArmorParser->endOfLineComment(pAppArmorParser)
 *  - void      pAppArmorParser->includeExpr(pAppArmorParser)
 *  - void      pAppArmorParser->variableAssign(pAppArmorParser)
 *  - void      pAppArmorParser->optionExpr(pAppArmorParser)
 *  - void      pAppArmorParser->profileExpr(pAppArmorParser)
 *  - ParseNode *      pAppArmorParser->ruleExpr(pAppArmorParser)
 *  - ParseNode *      pAppArmorParser->accessRule(pAppArmorParser)
 *  - AppArmorParser_accessPath_return      pAppArmorParser->accessPath(pAppArmorParser)
 *  - ParseNode *      pAppArmorParser->changeProfileRule(pAppArmorParser)
 *  - ParseNode *      pAppArmorParser->capabilityRule(pAppArmorParser)
 *  - ParseNode *      pAppArmorParser->capabilitiesSubExpr(pAppArmorParser)
 *  - ParseNode *      pAppArmorParser->auditRule(pAppArmorParser)
 *  - ParseNode *      pAppArmorParser->conditionalIfExpr(pAppArmorParser)
 *  - ParseNode *      pAppArmorParser->conditionalElseExpr(pAppArmorParser)
 *  - ParseNode *      pAppArmorParser->conditionalRules(pAppArmorParser)
 *  - ParseNode *      pAppArmorParser->builtInFunctions(pAppArmorParser)
 *  - AppArmorParser_ifExpr_return      pAppArmorParser->ifExpr(pAppArmorParser)
 *  - AppArmorParser_variableOrFunction_return      pAppArmorParser->variableOrFunction(pAppArmorParser)
 *  - AppArmorParser_andNotExpr_return      pAppArmorParser->andNotExpr(pAppArmorParser)
 *  - ParseNode *      pAppArmorParser->networkRule(pAppArmorParser)
 *  - char *      pAppArmorParser->domainRule(pAppArmorParser)
 *  - char *      pAppArmorParser->typeRule(pAppArmorParser)
 *  - char *      pAppArmorParser->protocolRule(pAppArmorParser)
 *  - AppArmorParser_networkActionRule_return      pAppArmorParser->networkActionRule(pAppArmorParser)
 *  - char *      pAppArmorParser->tcpRule(pAppArmorParser)
 *  - char *      pAppArmorParser->udpRule(pAppArmorParser)
 *  - AppArmorParser_networkHostRule_return      pAppArmorParser->networkHostRule(pAppArmorParser)
 *  - char *      pAppArmorParser->ipRule(pAppArmorParser)
 *  - char *      pAppArmorParser->ifaceRule(pAppArmorParser)
 *  - char *      pAppArmorParser->limitRule(pAppArmorParser)
 *
 * The return type for any particular rule is of course determined by the source
 * grammar file.
 */
#ifndef	_AppArmorParser_H
#define _AppArmorParser_H
/* =============================================================================
 * Standard antlr3 C runtime definitions
 */
#include    <antlr3.h>

/* End of standard antlr 3 runtime definitions
 * =============================================================================
 */

	#include "libaaprofileparse.h"
	#include "AppArmorLexer.h"
	#include "Exceptions.h"
	#include <stdio.h>


#ifdef	WIN32
// Disable: Unreferenced parameter,                - Rules with parameters that are not used
//          constant conditional,                  - ANTLR realizes that a prediction is always true (synpred usually)
//          initialized but unused variable        - tree rewrite vairables declared but not needed
//          Unreferenced local variable            - lexer rulle decalres but does not always use _type
//          potentially unitialized variable used  - retval always returned from a rule 
//
// These are only really displayed at warning level /W4 but that is the code ideal I am aiming at
// and the codegen must generate some of these warnings by necessity, apart from 4100, which is
// usually generated when a parser rule is given a parameter that it does not use. Mostly though
// this is a matter of orthogonality hence I disable that one.
//
#pragma warning( disable : 4100 )
#pragma warning( disable : 4101 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4189 )
#pragma warning( disable : 4701 )
#endif
typedef struct AppArmorParser_accessPath_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;   
    pANTLR3_STRING path;
    pANTLR3_STRING expandedPath;
}
    AppArmorParser_accessPath_return;

typedef struct AppArmorParser_ifExpr_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;   
    ConditionalExpr *expr;
    pANTLR3_STRING string;
}
    AppArmorParser_ifExpr_return;

typedef struct AppArmorParser_variableOrFunction_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;   
    ConditionalExpr *expr;
    pANTLR3_STRING string;
}
    AppArmorParser_variableOrFunction_return;

typedef struct AppArmorParser_andNotExpr_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;   
    ConditionalExpr *expr;
    pANTLR3_STRING string;
}
    AppArmorParser_andNotExpr_return;

typedef struct AppArmorParser_networkActionRule_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;   
    char *tcp;
    char *udp1;
    char *udp2;
}
    AppArmorParser_networkActionRule_return;

typedef struct AppArmorParser_networkHostRule_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;   
    char *direction;
    char *iprule;
}
    AppArmorParser_networkHostRule_return;





/** Context tracking structure for AppArmorParser
 */
typedef struct AppArmorParser_Ctx_struct
{
    /** Built in ANTLR3 context tracker contains all the generic elements
     *  required for context tracking.
     */
    pANTLR3_PARSER   pParser;

    ParseNode * (*startParse)	(struct AppArmorParser_Ctx_struct * ctx);
    void (*appArmorProfile)	(struct AppArmorParser_Ctx_struct * ctx);
    void (*newlineRule)	(struct AppArmorParser_Ctx_struct * ctx);
    void (*startOfLineComment)	(struct AppArmorParser_Ctx_struct * ctx);
    void (*endOfLineComment)	(struct AppArmorParser_Ctx_struct * ctx, Comment *a);
    void (*includeExpr)	(struct AppArmorParser_Ctx_struct * ctx);
    void (*variableAssign)	(struct AppArmorParser_Ctx_struct * ctx);
    void (*optionExpr)	(struct AppArmorParser_Ctx_struct * ctx);
    void (*profileExpr)	(struct AppArmorParser_Ctx_struct * ctx);
    ParseNode * (*ruleExpr)	(struct AppArmorParser_Ctx_struct * ctx);
    ParseNode * (*accessRule)	(struct AppArmorParser_Ctx_struct * ctx);
    AppArmorParser_accessPath_return (*accessPath)	(struct AppArmorParser_Ctx_struct * ctx);
    ParseNode * (*changeProfileRule)	(struct AppArmorParser_Ctx_struct * ctx);
    ParseNode * (*capabilityRule)	(struct AppArmorParser_Ctx_struct * ctx);
    ParseNode * (*capabilitiesSubExpr)	(struct AppArmorParser_Ctx_struct * ctx);
    ParseNode * (*auditRule)	(struct AppArmorParser_Ctx_struct * ctx);
    ParseNode * (*conditionalIfExpr)	(struct AppArmorParser_Ctx_struct * ctx);
    ParseNode * (*conditionalElseExpr)	(struct AppArmorParser_Ctx_struct * ctx);
    ParseNode * (*conditionalRules)	(struct AppArmorParser_Ctx_struct * ctx);
    ParseNode * (*builtInFunctions)	(struct AppArmorParser_Ctx_struct * ctx);
    AppArmorParser_ifExpr_return (*ifExpr)	(struct AppArmorParser_Ctx_struct * ctx);
    AppArmorParser_variableOrFunction_return (*variableOrFunction)	(struct AppArmorParser_Ctx_struct * ctx);
    AppArmorParser_andNotExpr_return (*andNotExpr)	(struct AppArmorParser_Ctx_struct * ctx);
    ParseNode * (*networkRule)	(struct AppArmorParser_Ctx_struct * ctx);
    char * (*domainRule)	(struct AppArmorParser_Ctx_struct * ctx);
    char * (*typeRule)	(struct AppArmorParser_Ctx_struct * ctx);
    char * (*protocolRule)	(struct AppArmorParser_Ctx_struct * ctx);
    AppArmorParser_networkActionRule_return (*networkActionRule)	(struct AppArmorParser_Ctx_struct * ctx);
    char * (*tcpRule)	(struct AppArmorParser_Ctx_struct * ctx);
    char * (*udpRule)	(struct AppArmorParser_Ctx_struct * ctx);
    AppArmorParser_networkHostRule_return (*networkHostRule)	(struct AppArmorParser_Ctx_struct * ctx);
    char * (*ipRule)	(struct AppArmorParser_Ctx_struct * ctx);
    char * (*ifaceRule)	(struct AppArmorParser_Ctx_struct * ctx);
    char * (*limitRule)	(struct AppArmorParser_Ctx_struct * ctx);    unsigned char * (*getGrammarFileName)();
    void	    (*free)   (struct AppArmorParser_Ctx_struct * ctx);
        
}
    AppArmorParser, * pAppArmorParser;

/* Function protoypes for the parser functions that external translation units
 * may wish to call.
 */
ANTLR3_API pAppArmorParser AppArmorParserNew         (pANTLR3_COMMON_TOKEN_STREAM     instream);/** Symbolic definitions of all the tokens that the parser will work with.
 * \{
 *
 * Antlr will define EOF, but we can't use that as it it is too common in
 * in C header files and that would be confusing. There is no way to filter this out at the moment
 * so we just undef it here for now. That isn't the value we get back from C recognizers
 * anyway. We are looking for ANTLR3_TOKEN_EOF.
 */
#ifdef	EOF
#undef	EOF
#endif
#ifdef	Tokens
#undef	Tokens
#endif 
#define LEFT_BRACE      20
#define GREATER_THAN      11
#define RIGHT_BRACE      14
#define LEFT_PAREN      18
#define EOL_COMMENT      5
#define IDENT      9
#define LIST_VAR_START      13
#define IDENT_START      28
#define WS      31
#define SOL_COMMENT      4
#define LESS_THAN      8
#define COMMENT_OR_INCLUDE      25
#define UNQUOTED_PATH      10
#define ANYTHING_NOT_QUOTED      26
#define COMMENT      24
#define DOLLAR      23
#define RULE      21
#define NEWLINE      6
#define EQUALS      16
#define IDENT_CONTINUE      29
#define BOOL_VAR_START      17
#define INCLUDE      7
#define QUOTED_STRING      12
#define EOF      -1
#define REAL_IDENT      30
#define PLUS      15
#define RIGHT_PAREN      19
#define AT      22
#define PATH      27
#ifdef	EOF
#undef	EOF
#define	EOF	ANTLR3_TOKEN_EOF
#endif

/* End of token definitions for AppArmorParser
 * =============================================================================
 */
/** \} */

#endif
/* END - Note:Keep extra linefeed to satisfy UNIX systems */
