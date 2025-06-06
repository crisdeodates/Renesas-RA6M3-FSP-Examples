/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2024  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.44 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : MENU_Private.h
Purpose     : Internal header file
---------------------------END-OF-HEADER------------------------------
*/

#ifndef MENU_PRIVATE_H
#define MENU_PRIVATE_H

#include "WM_Intern.h"
#include "WIDGET.h"
#include "MENU.h"
#include "GUI_ARRAY.h"

#if GUI_WINSUPPORT

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
/*********************************************************************
*
*       Status flags
*/
#define MENU_SF_HORIZONTAL              MENU_CF_HORIZONTAL
#define MENU_SF_VERTICAL                MENU_CF_VERTICAL
#define MENU_SF_OPEN_ON_POINTEROVER     MENU_CF_OPEN_ON_POINTEROVER
#define MENU_SF_CLOSE_ON_SECOND_CLICK   MENU_CF_CLOSE_ON_SECOND_CLICK
#define MENU_SF_HIDE_DISABLED_SEL       MENU_CF_HIDE_DISABLED_SEL

#define MENU_SF_ACTIVE                  (1 << 6)  // Internal flag only
#define MENU_SF_POPUP                   (1 << 7)  // Internal flag only
#define MENU_SF_ARROW                   (1 << 8)  // Internal flag only

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
//
// MENU_SKIN_PRIVATE
//
typedef struct {
  WIDGET_DRAW_ITEM_FUNC * pfDrawSkin;
} MENU_SKIN_PRIVATE;

//
// MENU_ITEM
//
typedef struct {
  MENU_Handle hSubmenu;
  U16         Id;
  U16         Flags;
  int         TextWidth;
  char        acText[1];
} MENU_ITEM;

//
// MENU_PROPS
//
typedef struct {
  GUI_COLOR           aTextColor[5];
  GUI_COLOR           aBkColor[5];
  U8                  aBorder[4];
  const GUI_FONT    * pFont;
  MENU_SKIN_PRIVATE   SkinPrivate;
} MENU_PROPS;

//
// MENU_Obj
//
typedef struct {
  WIDGET              Widget;
  MENU_PROPS          Props;
  GUI_ARRAY           ItemArray;
  WM_HWIN             hOwner;
  U16                 Flags;
  char                IsSubmenuActive;
  int                 Width;
  int                 Height;
  int                 Sel;
  unsigned            ArrowAreaWidth;
  WIDGET_SKIN const * pWidgetSkin;
} MENU_Obj;

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define MENU_INIT_ID(pObj)  (pObj->Widget.DebugId = WIDGET_TYPE_MENU)
#else
  #define MENU_INIT_ID(pObj)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  MENU_Obj * MENU_LockH(MENU_Handle hObj);
  #define MENU_LOCK_H(hObj)   MENU_LockH(hObj)
#else
  #define MENU_LOCK_H(hObj)   (MENU_Obj *)WM_LOCK_H(hObj)
#endif

/*********************************************************************
*
*       Public data (internal defaults)
*
**********************************************************************
*/

extern MENU_PROPS            MENU__DefaultProps;
extern const WIDGET_EFFECT * MENU__pDefaultEffect;

extern const WIDGET_SKIN     MENU__SkinClassic;
extern       WIDGET_SKIN     MENU__Skin;

extern WIDGET_SKIN const   * MENU__pSkinDefault;

/*********************************************************************
*
*       Private functions
*
**********************************************************************
*/
int       MENU__CalcMenuSizeX         (MENU_Handle hObj);
int       MENU__CalcMenuSizeY         (MENU_Handle hObj);
int       MENU__FindItem              (MENU_Handle hObj, U16 ItemId, MENU_Handle* phMenu);
int       MENU__GetEffectSize         (MENU_Handle hObj);
int       MENU__GetItemHeight         (MENU_Handle hObj, unsigned Index);
int       MENU__GetItemWidth          (MENU_Handle hObj, unsigned Index);
unsigned  MENU__GetNumItems           (MENU_Obj * pObj);
int       MENU__HasEffect             (MENU_Handle hObj, MENU_Obj * pObj);
void      MENU__InvalidateItem        (MENU_Handle hObj, unsigned Index);
void      MENU__RecalcTextWidthOfItems(MENU_Obj * pObj);
void      MENU__ResizeMenu            (MENU_Handle hObj);
int       MENU__SendMenuMessage       (MENU_Handle hObj, WM_HWIN hDestWin, U16 MsgType, U16 ItemId);
char      MENU__SetItem               (MENU_Handle hObj, unsigned Index, const MENU_ITEM_DATA* pItemData);
void      MENU__SetItemFlags          (MENU_Obj * pObj, unsigned Index, U16 Mask, U16 Flags);

#endif  // GUI_WINSUPPORT
#endif  // MENU_PRIVATE_H

/*************************** End of file ****************************/
