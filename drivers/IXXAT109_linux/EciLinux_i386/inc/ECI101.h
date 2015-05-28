///////////////////////////////////////////////////////////////////////////////
// (C) 2008-2011 IXXAT Automation GmbH, all rights reserved
///////////////////////////////////////////////////////////////////////////////
/**
  ECI API function redefintions for IXXAT Interface: iPC-I 320/PCI.

  Headerfile of the ECI ( embedded / realtime communication interface), 
  a generic library for IXXAT hardware interfaces.

  @file ECI101.h
*/

#ifndef __ECI101_H__
#define __ECI101_H__

//*** At first undefine maybe previously defined ECI export functions
#undef ECIDRV_                     
#undef ECIDRV_Initialize           
#undef ECIDRV_Release              
#undef ECIDRV_GetInfo              
#undef ECIDRV_CtrlOpen             
#undef ECIDRV_CtrlClose            
#undef ECIDRV_CtrlStart            
#undef ECIDRV_CtrlStop             
#undef ECIDRV_CtrlGetCapabilities  
#undef ECIDRV_CtrlGetStatus        
#undef ECIDRV_CtrlSend             
#undef ECIDRV_CtrlReceive          
#undef ECIDRV_CtrlSetAccFilter
#undef ECIDRV_CtrlAddFilterIds        
#undef ECIDRV_CtrlRemFilterIds        
#undef ECIDRV_CtrlCommand          
#undef ECIDRV_LogConfig            
#undef ECIDRV_LogRead              
#undef ECIDRV_LogStart             
#undef ECIDRV_LogStop              
#undef ECIDRV_LogClear             
#undef ECIDRV_GetErrorString       

  
//*** Define ECI export functions for defined hardware type
#define ECIDRV_                     ECI101_                   
#define ECIDRV_Initialize           ECI101_Initialize         
#define ECIDRV_Release              ECI101_Release            
#define ECIDRV_GetInfo              ECI101_GetInfo            
#define ECIDRV_CtrlOpen             ECI101_CtrlOpen           
#define ECIDRV_CtrlClose            ECI101_CtrlClose          
#define ECIDRV_CtrlStart            ECI101_CtrlStart          
#define ECIDRV_CtrlStop             ECI101_CtrlStop           
#define ECIDRV_CtrlGetCapabilities  ECI101_CtrlGetCapabilities
#define ECIDRV_CtrlGetStatus        ECI101_CtrlGetStatus      
#define ECIDRV_CtrlSend             ECI101_CtrlSend           
#define ECIDRV_CtrlReceive          ECI101_CtrlReceive        
#define ECIDRV_CtrlSetAccFilter     ECI101_CtrlSetAccFilter
#define ECIDRV_CtrlAddFilterIds     ECI101_CtrlAddFilterIds
#define ECIDRV_CtrlRemFilterIds     ECI101_CtrlRemFilterIds       
#define ECIDRV_CtrlCommand          ECI101_CtrlCommand        
#define ECIDRV_LogConfig            ECI101_LogConfig          
#define ECIDRV_LogRead              ECI101_LogRead            
#define ECIDRV_LogStart             ECI101_LogStart           
#define ECIDRV_LogStop              ECI101_LogStop            
#define ECIDRV_LogClear             ECI101_LogClear           
#define ECIDRV_GetErrorString       ECI101_GetErrorString     


/** @def ECIDRV_
  General ECI API function renaming */
/** @def ECIDRV_Initialize 
  @brief @copybrief ECIDRV_Initialize @n <b> See function:</b> @ref ECIDRV_Initialize */
/** @def ECIDRV_Release 
  @brief @copybrief ECIDRV_Release @n <b> See function:</b> @ref ECIDRV_Release */
/** @def ECIDRV_GetInfo 
  @brief @copybrief ECIDRV_GetInfo @n <b> See function:</b> @ref ECIDRV_GetInfo */
/** @def ECIDRV_CtrlOpen 
  @brief @copybrief ECIDRV_CtrlOpen @n <b> See function:</b> @ref ECIDRV_CtrlOpen */
/** @def ECIDRV_CtrlClose 
  @brief @copybrief ECIDRV_CtrlClose @n <b> See function:</b> @ref ECIDRV_CtrlClose */
/** @def ECIDRV_CtrlStart 
  @brief @copybrief ECIDRV_CtrlStart @n <b> See function:</b> @ref ECIDRV_CtrlStart */
/** @def ECIDRV_CtrlStop 
  @brief @copybrief ECIDRV_CtrlStop @n <b> See function:</b> @ref ECIDRV_CtrlStop */
/** @def ECIDRV_CtrlGetCapabilities 
  @brief @copybrief ECIDRV_CtrlGetCapabilities @n <b> See function:</b> @ref ECIDRV_CtrlGetCapabilities */
/** @def ECIDRV_CtrlGetStatus 
  @brief @copybrief ECIDRV_CtrlGetStatus @n <b> See function:</b> @ref ECIDRV_CtrlGetStatus */
/** @def ECIDRV_CtrlSend 
  @brief @copybrief ECIDRV_CtrlSend @n <b> See function:</b> @ref ECIDRV_CtrlSend */
/** @def ECIDRV_CtrlReceive 
  @brief @copybrief ECIDRV_CtrlReceive @n <b> See function:</b> @ref ECIDRV_CtrlReceive */
/** @def ECIDRV_CtrlSetAccFilter 
  @brief @copybrief ECIDRV_CtrlSetAccFilter @n <b> See function:</b> @ref ECIDRV_CtrlSetAccFilter */
/** @def ECIDRV_CtrlAddFilterIds 
  @brief @copybrief ECIDRV_CtrlAddFilterIds @n <b> See function:</b> @ref ECIDRV_CtrlAddFilterIds */
/** @def ECIDRV_CtrlRemAccFilter 
  @brief @copybrief ECIDRV_CtrlRemAccFilter @n <b> See function:</b> @ref ECIDRV_CtrlRemAccFilter */    
/** @def ECIDRV_CtrlCommand 
  @brief @copybrief ECIDRV_CtrlCommand @n <b> See function:</b> @ref ECIDRV_CtrlCommand */
/** @def ECIDRV_LogConfig 
  @brief @copybrief ECIDRV_LogConfig @n <b> See function:</b> @ref ECIDRV_LogConfig */
/** @def ECIDRV_LogRead 
  @brief @copybrief ECIDRV_LogRead @n <b> See function:</b> @ref ECIDRV_LogRead */
/** @def ECIDRV_LogStart 
  @brief @copybrief ECIDRV_LogStart @n <b> See function:</b> @ref ECIDRV_LogStart */
/** @def ECIDRV_LogStop 
  @brief @copybrief ECIDRV_LogStop @n <b> See function:</b> @ref ECIDRV_LogStop */
/** @def ECIDRV_LogClear 
  @brief @copybrief ECIDRV_LogClear @n <b> See function:</b> @ref ECIDRV_LogClear */
/** @def ECIDRV_GetErrorString 
  @brief @copybrief ECIDRV_GetErrorString @n <b> See function:</b> @ref ECIDRV_GetErrorString */


//////////////////////////////////////////////////////////////////////////
// include files
#include <ECI.h>


#endif //__ECI101_H__
