#pragma once
#include "Task_Empty.h" //적기 따라가는 코드로 고침
#include "Task_Pure.h"
#include "Task_FollowTarget.h" // 적기를 따라감
#include "Task_ClimbToSafeAltitude.h" //1000ft 이상 기동

#include "Task_AntiOvershoot.h" // OBFM기동
#include "Task_LeadPursuit.h" // OBFM 기동
#include "Task_EvasiveRollOrScissors.h" // DBFM 방어 기동
#include "Task_CounterTurn.h" // DBFM 방어 기동
#include "Task_RollReverseAttack.h" // DBFM 반격 기동
#include "Task_OneCircleAttack.h" // HABFM 기동
#include "Task_TwoCircleAttack.h" // HABFM 기동
#include "Task_ScissorBreakTurn.h" // Scissor 기동
#include "Task_ScissorRollBack.h" // Scissor 기동

#include "Task_MakeLOS.h"
#include "Task_CloseDistance.h"
#include "Task_MinimizeAngleOff.h"