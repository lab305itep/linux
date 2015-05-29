#ifndef _UAPI_LINUX_VME
#define _UAPI_LINUX_VME

/* VME Address Spaces */
#define VME_A16		0x1
#define VME_A24		0x2
#define VME_A32		0x4
#define VME_A64		0x8
#define VME_CRCSR	0x10
#define VME_USER1	0x20
#define VME_USER2	0x40
#define VME_USER3	0x80
#define VME_USER4	0x100

#define VME_A16_MAX	0x10000ULL
#define VME_A24_MAX	0x1000000ULL
#define VME_A32_MAX	0x100000000ULL
#define VME_A64_MAX	0x10000000000000000ULL
#define VME_CRCSR_MAX	0x1000000ULL

/* VME Cycle Types */
#define VME_SCT		0x1
#define VME_BLT		0x2
#define VME_MBLT	0x4
#define VME_2eVME	0x8
#define VME_2eSST	0x10
#define VME_2eSSTB	0x20

#define VME_2eSST160	0x100
#define VME_2eSST267	0x200
#define VME_2eSST320	0x400

#define VME_SUPER	0x1000
#define VME_USER	0x2000
#define VME_PROG	0x4000
#define VME_DATA	0x8000

/* VME Data Widths */
#define VME_D8		0x1
#define VME_D16		0x2
#define VME_D32		0x4
#define VME_D64		0x8

/* VME Transfer Directions */
#define VME_DMA_VME_TO_MEM		(1 << 0)
#define VME_DMA_MEM_TO_VME		(1 << 1)
#define VME_DMA_VME_TO_VME		(1 << 2)
#define VME_DMA_MEM_TO_MEM		(1 << 3)
#define VME_DMA_PATTERN_TO_VME		(1 << 4)
#define VME_DMA_PATTERN_TO_MEM		(1 << 5)

#define VME_DMA_PATTERN_BYTE		(1 << 0)
#define VME_DMA_PATTERN_WORD		(1 << 1)
#define VME_DMA_PATTERN_INCREMENT	(1 << 2)

#endif
