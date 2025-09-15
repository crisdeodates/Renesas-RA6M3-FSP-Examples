/***********************************************************************************************************************
 * File Name    : sender_task_entry.c
 * Description  : Contains function definitions.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "sender_task.h"
#include "common_utils.h"
#include "freertos_ep.h"
#include "tau_timer_initialize.h"

/*******************************************************************************************************************//**
 * @addtogroup freertos_ep
 * @{
 **********************************************************************************************************************/

/* Flag to check Message Queue task completion */
static bool b_suspend_msqQ_task = false;

/* Sender Task entry function */
/* pvParameters contains TaskHandle_t */
void sender_task_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    /* Variable to store task state */
    eTaskState Task_State = (eTaskState) RESET_VALUE;

    fsp_pack_version_t version = {RESET_VALUE};

    /* Initialize the message to send to Message Queue */
    msg_t message_to_task = {100, "Sender_Task"};

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the RTT */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
              version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* The rate at which the task waits on the Message availability */
    TickType_t Queue_wait_ticks = pdMS_TO_TICKS(WAIT_TIME);

    /* Initialize the variable used by the call to vTaskDelayUntil() */
    TickType_t last_execution_time = xTaskGetTickCount();

    /* Check the task state, if in running or ready state only,
     * suspend Semaphore task. Semaphore task will be resumed after
     * Message Queue task has completed execution */
    Task_State = eTaskGetState(semaphore_task);
    if ((eBlocked == Task_State) || (eRunning == Task_State) || (eReady == Task_State))
    {
        vTaskSuspend(semaphore_task);
    }

    APP_PRINT("\r\nSender_Task: Starting g_periodic_timer_msgq timer");
    /* Start TAU timer to start sending message to Message Queue periodically at 1 second from TAU callback */
    fsp_err_t fsp_err = tau_timer_init(&g_periodic_timer_msgq_ctrl, &g_periodic_timer_msgq_cfg );
    /* Handle error */
    if(FSP_SUCCESS != fsp_err)
    {
        APP_ERR_TRAP(fsp_err);
    }

    while (true)
    {
        /* Check if flag is set, to suspend Message Queue task. If not set,
         * continue sending messages to Message Queue at 500ms periodically */
        if (true != b_suspend_msqQ_task)
        {

            /* Send message to back of Message Queue */
            if (pdPASS == xQueueSendToBack (g_queue, &message_to_task, Queue_wait_ticks))
            {
                APP_PRINT("\r\nSender_Task: Message posted on Queue successfully");
                APP_PRINT("\r\nSender_Task: Going on delay for 500ms");

                /* Places the task into the Blocked state. The task
                 * will execute every 500ms once */
                vTaskDelayUntil(&last_execution_time , WAIT_TIME);

                APP_PRINT("\r\nSender_Task: After delay of 500ms\r\n");
            }
        }
        else
        {
            /* Flush the messages from the queue before suspending the task */
            if (pdPASS == xQueueReset(g_queue))
            {
                APP_PRINT("\r\nSender_Task: Message Queue reset\r\n");

                /* Suspend Message Queue tasks and resume Semaphore task for Execution */
                fsp_err = R_TAU_Stop(&g_periodic_timer_msgq_ctrl);
                /* Handle error */
                if (FSP_SUCCESS != fsp_err)
                {
                    if (FSP_SUCCESS != R_TAU_Close(&g_periodic_timer_msgq_ctrl))
                    {
                        /* Print out in case of error */
                        APP_ERR_PRINT("\r\nSender_Task: R_TAU_Close API failed\r\n");
                    }
                    /* Print out in case of error */
                    APP_ERR_PRINT("\r\nSender_Task: R_TAU_Stop API failed\r\n");
                    APP_ERR_TRAP(fsp_err);
                }
                APP_PRINT("\r\nSender_Task: TAU timer stopped\r\n");

                fsp_err = R_TAU_Close(&g_periodic_timer_msgq_ctrl);
                if (FSP_SUCCESS != fsp_err)
                {
                    /* Print out in case of error */
                    APP_ERR_PRINT("\r\nSender_Task: R_TAU_Close API failed\r\n");
                    APP_ERR_TRAP(fsp_err);
                }

                vTaskSuspend(receiver_task);
                APP_PRINT("\r\nSender_Task: Receiver Task Suspended\r\n");

                /* Before suspending current task resume the semaphore task */
                vTaskResume(semaphore_task);
                APP_PRINT("\r\nSender_Task: Semaphore Task Resumed\r\n");
                APP_PRINT("\r\nSender_Task: Suspending Sender Task\r\n");

                /* Suspends calling task */
                vTaskSuspend(RESET_VALUE);
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief      User-defined TAU IRQ callback for the Message Queue.
 * @param[in]  p_args  Callback function parameter data.
 * @retval     None
 **********************************************************************************************************************/
void periodic_timer_msgq_cb(timer_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    /* Counter to track task suspend count */
    static uint8_t msgQ_counter = RESET_VALUE;

    /* Variable is set to true if the priority of the unblocked task is higher
     * than that of the task that was running when the interrupt occurred */
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* Initialize the message to send to Message Queue */
    static msg_t message_to_task = {200, "TAU Callback"};

    /* If the ISR has executed for 10 seconds, set the flag to suspend the sender task */
    if(TASK_SUSPEND_COUNT == msgQ_counter)
    {
        /* Set flag to suspend Message Queue task */
        b_suspend_msqQ_task = true;
    }
    else
    {
        /* Send message to Message Queue */
        if (pdPASS == xQueueSendToBackFromISR(g_queue, &message_to_task, &xHigherPriorityTaskWoken))
        {
            /* Increment counter on successfully sending message */
            msgQ_counter++;
        }
    }
    /* A context switch will be performed if xHigherPriorityTaskWoken is set to pdTRUE */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/*******************************************************************************************************************//**
 * @} (end addtogroup freertos_ep)
 **********************************************************************************************************************/
