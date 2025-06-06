#ifndef R_SCI_B_UART_CFG_H_
#define R_SCI_B_UART_CFG_H_

#if (USE_SERIAL_TERM == 1)
#ifdef __cplusplus
            extern "C" {
            #endif

#define SCI_B_UART_CFG_PARAM_CHECKING_ENABLE (BSP_CFG_PARAM_CHECKING_ENABLE)
#define SCI_B_UART_CFG_FIFO_SUPPORT (0)
#define SCI_B_UART_CFG_DTC_SUPPORTED (0)
#define SCI_B_UART_CFG_FLOW_CONTROL_SUPPORT (0)

#ifdef __cplusplus
            }
            #endif

#endif /* USE_SERIAL_TERM */
#endif
