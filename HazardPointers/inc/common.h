/* Define this Macro if Memory Management is needed */
//#define Enable_Hazard_Pointer

/* Define this Macro if you wan to test stack
 * else Queue will be selected as defualt data structure
 */
#define ENABLE_STACK_OPER

#ifdef ENABLE_STACK_OPER
#define HAZARD_POINTER 1
#else
#define HAZARD_POINTER 2
#endif