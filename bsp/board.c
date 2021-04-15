
#include <stdint.h>
#include <rthw.h>
#include <rtthread.h>
#include "cy_device_headers.h"
#include "board.h"
#include "cy_systick.h"
#include "cycfg.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

extern int entry(void);

#define configTOTAL_HEAP_SIZE           (32*1024)
/* Allocate the memory for the heap. */
ALIGN(RT_ALIGN_SIZE)
static uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
/**
 * This is the timer interrupt service routine.
 *
 */
void SysTick_Handler_CB(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}


 /* RT-Thread OS take over the MCU*/
void cy_toolchain_init(void)
{
	entry();
}

#ifdef RT_USING_CONSOLE
/*Achieve the RT CONSOLE output with BSP UART*/
void rt_hw_console_output(const char *str)
{
    /*console output */
	printf("%s\r\n", str);

}
#endif

/*BSP UART initial*/
cy_rslt_t Retarget_Io_Init(void)
{
	cy_rslt_t ret;
	ret = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
	return ret;
}


void rt_hw_board_init()
{
	cy_rslt_t result;
	/* Initialize system resource */
     cybsp_init();

     __enable_irq();

     SystemCoreClockUpdate();

	/* Initialize systick */
	Cy_SysTick_Init(CY_SYSTICK_CLOCK_SOURCE_CLK_CPU, SystemCoreClock/RT_TICK_PER_SECOND);
	Cy_SysTick_SetCallback(0, SysTick_Handler_CB);
	Cy_SysTick_EnableInterrupt();

	rt_system_heap_init((void*)ucHeap, (void*)(ucHeap+configTOTAL_HEAP_SIZE));

	/*Initial the MCU UART*/
	result = Retarget_Io_Init();
	    if(result != CY_RSLT_SUCCESS)
	    {
	    	CY_ASSERT(0);
	    }
	    else
	    {
	    	printf("UART initial OK!\r\n");
	    }
}

/*@}*/
