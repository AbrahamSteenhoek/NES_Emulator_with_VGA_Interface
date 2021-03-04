/*****************************************************************************
 * Joseph Zambreno
 * Phillip Jones
 * Department of Electrical and Computer Engineering
 * Iowa State University
 *****************************************************************************/

/*****************************************************************************
 * vga_test.c - example VGA out using a v_tc timing controller and vdma
 * module.
 *
 *
 * NOTES:
 * 10/11/13 by JAZ::Design created.
 * 1/15/2018 by PHJ: Update XVtc_Enable, new function that take only one arg
 * 1/16/2018 by PHJ: Upaded to provide students with an option for directly
                     accessing registers via pointers.
 *****************************************************************************/

#include "xil_printf.h"
#include "platform.h"
#include "xparameters.h"
#include "xil_cache.h"
#include "xaxivdma.h"
#include "xvtc.h"
#include "sleep.h"

#define HSIZE 640
#define VSIZE 480

u16 test_image[VSIZE][HSIZE];

void PopulateCheckerboardImg( u16 img[VSIZE][HSIZE] )
{
	const u16 red = 0x41C0;
	const u16 gold = 0x4BE0;
	const u16 black = 0x0000;

	int i ,j;
	for (i = 0; i < VSIZE; i++) {
	  for (j = 0; j < HSIZE; j++) {
		// even row
		if(i%120 < 60)
			if(j < 3 || j > 636)
				img[i][j] = black;
			else if(j% 160 < 80)
				img[i][j] = red; //R
			else
				img[i][j] = gold; //Y
		// odd row
		else
			if(j < 1 || j > 638)
				img[i][j] = black;
			else if(j% 160 < 80)
				img[i][j] = gold; //Y
			else
				img[i][j] = red; //R
		  }
	}
}

void PopulateRGBColsImg( u16 img[VSIZE][HSIZE] )
{

	int i ,j;
	for (i = 0; i < VSIZE; i++) {
	  for (j = 0; j < HSIZE; j++) {

		if (j < 213) {
			img[i][j] = 0x00F0; // red pixels
		}
		else if(j < 426 ) {
			img[i][j] = 0x0F00; // green pixels
		}
		else {
			img[i][j] = 0xF000; // blue pixels
		}

	  }
	}
}



int main() {

	XVtc Vtc;
    XVtc_Config *VtcCfgPtr;

    print("Init VTC\r\n");
    // Enable VTC module: Using high-level functions provided by Vendor
    VtcCfgPtr = XVtc_LookupConfig(XPAR_V_TC_0_DEVICE_ID);
    XVtc_CfgInitialize(&Vtc, VtcCfgPtr, VtcCfgPtr->BaseAddress);
    XVtc_EnableGenerator(&Vtc);

    // Challenge: Can you rewrite the Enable VTC module code by directly accessing
    // the VTC registers using pointers?  (See VTC data sheet, and xparameters.h)

    // VTC_CR[0]   |=  ENABLE_VTC;    // You: Declare VTC_CR and ENABLE_VTC appropriately (before main() )

    print( "Populating test image...\r\n" );
    PopulateCheckerboardImg( test_image );

	// Make sure Display information gets flushed from cache to DDR Memory
    Xil_DCacheFlush();

    // Set up VDMA config registers
	#define CHANGE_ME 0
    print("Write VDMA regs\r\n");
    // Simple function abstraction by Vendor for writing VDMA registers
    XAxiVdma_WriteReg(XPAR_AXI_VDMA_0_BASEADDR, XAXIVDMA_CR_OFFSET,  0x0003);  // Read Channel: VDMA MM2S Circular Mode and Start bits set, VDMA MM2S Control
    XAxiVdma_WriteReg(XPAR_AXI_VDMA_0_BASEADDR, XAXIVDMA_HI_FRMBUF_OFFSET, 0x0);  // Read Channel: VDMA MM2S Reg_Index
    XAxiVdma_WriteReg(XPAR_AXI_VDMA_0_BASEADDR, XAXIVDMA_MM2S_ADDR_OFFSET + XAXIVDMA_START_ADDR_OFFSET, test_image);  // Read Channel: VDMA MM2S Frame buffer Start Addr 1
    XAxiVdma_WriteReg(XPAR_AXI_VDMA_0_BASEADDR, XAXIVDMA_MM2S_ADDR_OFFSET + XAXIVDMA_STRD_FRMDLY_OFFSET, 0x0500);  // Read Channel: VDMA MM2S FRM_Delay, and Stride
    XAxiVdma_WriteReg(XPAR_AXI_VDMA_0_BASEADDR, XAXIVDMA_MM2S_ADDR_OFFSET + XAXIVDMA_HSIZE_OFFSET, 0x0500);  // Read Channel: VDMA MM2S HSIZE
    XAxiVdma_WriteReg(XPAR_AXI_VDMA_0_BASEADDR, XAXIVDMA_MM2S_ADDR_OFFSET + XAXIVDMA_VSIZE_OFFSET, 0x1E0);  // Read Channel: VDMA MM2S VSIZE  (Note: Also Starts VDMA transaction)


    // Low-level register acess using pointers
    // Alternative approach for configuring VDMA registers: Instead of using the abstracted functions can you configure and start the VDMA using pointers to directly configure VDMA registers
    // YOU: Declare VDMA_MM2S_XXX (before main) and set values "CHANGE_ME" appropriately, before main()

    // VDMA_MM2S_CR[0]             = CHANGE_ME; // Read Channel: VDMA MM2S Circular/Park Mode and enable the channel
    // VDMA_MM2S_REG_INDEX[0]      = CHANGE_ME; // Read Channel: VDMA MM2S Reg_Index
    // VDMA_MM2S_START_ADDRESS1[0] = CHANGE_ME; // Read channel: VDMA MM2S Frame buffer Start Add 1
    // VDMA_MM2S_FRMDLY_STRIDE[0]  = CHANGE_ME; // Read channel: VDMA MM2S FRM_Delay, and Stride
    // VDMA_MM2S_HSIZE[0]          = CHANGE_ME; // Read channel: VDMA MM2S HSIZE
    // VDMA_MM2S_VSIZE[0]          = CHANGE_ME; // Read channel: VDMA MM2S VSIZE  (Note: Also Starts VDMA transaction)

    print("spinning...\r\n");
    while(1){    }
    ////////////////////////////////////////////////////////////////
    // Xilinx clean up function
    ////////////////////////////////////////////////////////////////
    cleanup_platform();

    return 0;
}
