#define VI_NTSC_CLOCK 48681812 // NTSC: Hz = 48.681812 MHz
#define VI_PAL_CLOCK  49656530 //  PAL: Hz = 49.656530 MHz
#define VI_MPAL_CLOCK 48628316 // MPAL: Hz = 48.628316 MHz

extern volatile struct { // RDRAM
  uint32_t DEVICE_TYPE;  // 03F00000..03F00003 RDRAM: Device Type
  uint32_t DEVICE_ID;    // 03F00004..03F00007 RDRAM: Device ID
  uint32_t DELAY;        // 03F00008..03F0000B RDRAM: Delay
  uint32_t MODE;         // 03F0000C..03F0000F RDRAM: Mode
  uint32_t REF_INTERVAL; // 03F00010..03F00013 RDRAM: Ref Interval
  uint32_t REF_ROW;      // 03F00014..03F00017 RDRAM: Ref Row
  uint32_t RAS_INTERVAL; // 03F00018..03F0001B RDRAM: Ras Interval
  uint32_t MIN_INTERVAL; // 03F0001C..03F0001F RDRAM: Minimum Interval
  uint32_t ADDR_SELECT;  // 03F00020..03F00023 RDRAM: Address Select
  uint32_t DEVICE_MANUF; // 03F00024..03F00027 RDRAM: Device Manufacturer
} RDRAM;

extern const volatile struct { // RSP Memory (SP MEM)
  uint8_t DMEM[0x1000]; // 04000000..04000FFF SP MEM: RSP DMEM (4096 Bytes)
  uint8_t IMEM[0x1000]; // 04001000..04001FFF SP MEM: RSP IMEM (4096 Bytes)
} SP_MEM;

extern volatile struct { // RSP (SP)
  uint32_t MEM_ADDR;  // 04040000..04040003 SP: Master, SP Memory Address
  uint32_t DRAM_ADDR; // 04040004..04040007 SP: Slave, SP DRAM DMA Address
  uint32_t RD_LEN;    // 04040008..0404000B SP: Read DMA Length
  uint32_t WR_LEN;    // 0404000C..0404000F SP: Write DMA Length
  uint32_t STATUS;    // 04040010..04040013 SP: Status
  uint32_t DMA_FULL;  // 04040014..04040017 SP: DMA Full
  uint32_t DMA_BUSY;  // 04040018..0404001B SP: DMA Busy
  uint32_t SEMAPHORE; // 0404001C..0404001F SP: Semaphore
} SP;

extern volatile struct { // RSP Program Counter (SP PC)
  uint32_t PC;       // 04080000..04080003 SP PC: PC
  uint32_t SP_IBIST; // 04080004..04080007 SP PC: IMEM BIST
} SP_PC;

extern volatile struct { // RDP Command (DPC)
  uint32_t START;    // 04100000..04100003 DPC: CMD DMA Start
  uint32_t END;      // 04100004..04100007 DPC: CMD DMA End
  uint32_t CURRENT;  // 04100008..0410000B DPC: CMD DMA Current
  uint32_t STATUS;   // 0410000C..0410000F DPC: CMD Status
  uint32_t CLOCK;    // 04100010..04100013 DPC: Clock Counter
  uint32_t BUFBUSY;  // 04100014..04100017 DPC: Buffer Busy Counter
  uint32_t PIPEBUSY; // 04100018..0410001B DPC: Pipe Busy Counter
  uint32_t TMEM;     // 0410001C..0410001F DPC: TMEM Load Counter
} DPC;

extern volatile struct { // RDP Span (DPS)
  uint32_t TBIST;        // 04200000..04200003 DPS: Tmem Bist
  uint32_t TEST_MODE;    // 04200004..04200007 DPS: Span Test Mode
  uint32_t BUFTEST_ADDR; // 04200008..0420000B DPS: Span Buffer Test Address
  uint32_t BUFTEST_DATA; // 0420000C..0420000F DPS: Span Buffer Test Data
} DPS;

extern volatile struct { // MIPS Interface (MI)
  uint32_t INIT_MODE; // 04300000..04300003 MI: Init Mode
  uint32_t VERSION;   // 04300004..04300007 MI: Version
  uint32_t INTR;      // 04300008..0430000B MI: Interrupt
  uint32_t INTR_MASK; // 0430000C..0430000F MI: Interrupt Mask
} MI;

extern volatile struct { // Video Interface (VI)
  uint32_t STATUS;         // 04400000..04400003 VI: Status/Control
  uint32_t ORIGIN;         // 04400004..04400007 VI: Origin
  uint32_t WIDTH;          // 04400008..0440000B VI: Width
  uint32_t V_INTR;         // 0440000C..0440000F VI: Vertical Interrupt
  uint32_t V_CURRENT_LINE; // 04400010..04400013 VI: Current Vertical Line
  uint32_t TIMING;         // 04400014..04400017 VI: Video Timing
  uint32_t V_SYNC;         // 04400018..0440001B VI: Vertical Sync
  uint32_t H_SYNC;         // 0440001C..0440001F VI: Horizontal Sync
  uint32_t H_SYNC_LEAP;    // 04400020..04400023 VI: Horizontal Sync Leap
  uint32_t H_VIDEO;        // 04400024..04400027 VI: Horizontal Video
  uint32_t V_VIDEO;        // 04400028..0440002B VI: Vertical Video
  uint32_t V_BURST;        // 0440002C..0440002F VI: Vertical Burst
  uint32_t X_SCALE;        // 04400030..04400033 VI: X-Scale
  uint32_t Y_SCALE;        // 04400034..04400037 VI: Y-Scale
} VI;

extern volatile struct { // Audio Interface (AI)
  uint32_t DRAM_ADDR; // 04500000..04500003 AI: DRAM Address
  uint32_t LEN;       // 04500004..04500007 AI: Length
  uint32_t CONTROL;   // 04500008..0450000B AI: Control
  uint32_t STATUS;    // 0450000C..0450000F AI: Status
  uint32_t DACRATE;   // 04500010..04500013 AI: DAC Sample Period
  uint32_t BITRATE;   // 04500014..04500017 AI: Bit Rate
} AI;

extern volatile struct { // Peripheral Interface (PI)
  uint32_t DRAM_ADDR;    // 04600000..04600003 PI: DRAM Address
  uint32_t CART_ADDR;    // 04600004..04600007 PI: Pbus (Cartridge) Address
  uint32_t RD_LEN;       // 04600008..0460000B PI: Read Length
  uint32_t WR_LEN;       // 0460000C..0460000F PI: Write length
  uint32_t STATUS;       // 04600010..04600013 PI: Status
  uint32_t BSD_DOM1_LAT; // 04600014..04600017 PI: Domain 1 Latency
  uint32_t BSD_DOM1_PWD; // 04600018..0460001B PI: Domain 1 Pulse Width
  uint32_t BSD_DOM1_PGS; // 0460001C..0460001F PI: Domain 1 Page Size
  uint32_t BSD_DOM1_RLS; // 04600020..04600023 PI: Domain 1 Release
  uint32_t BSD_DOM2_LAT; // 04600024..04600027 PI: Domain 2 Latency
  uint32_t BSD_DOM2_PWD; // 04600028..0460002B PI: Domain 2 Pulse Width
  uint32_t BSD_DOM2_PGS; // 0460002C..0460002F PI: Domain 2 Page Size
  uint32_t BSD_DOM2_RLS; // 04600030..04600033 PI: Domain 2 Release
} PI;

extern volatile struct { // RDRAM Interface (RI)
  uint32_t MODE;         // 04700000..04700003 RI: Mode
  uint32_t CONFIG;       // 04700004..04700007 RI: Config
  uint32_t CURRENT_LOAD; // 04700008..0470000B RI: Current Load
  uint32_t SELECT;       // 0470000C..0470000F RI: Select
  uint32_t REFRESH;      // 04700010..04700013 RI: Refresh
  uint32_t LATENCY;      // 04700014..04700017 RI: Latency
  uint32_t RERROR;       // 04700018..0470001B RI: Read Error
  uint32_t WERROR;       // 0470001C..0470001F RI: Write Error
} RI;

extern volatile struct { // Serial Interface (SI)
  uint32_t DRAM_ADDR;      // 04800000..04800003 SI: DRAM Address
  uint32_t PIF_ADDR_RD64B; // 04800004..04800007 SI: Address Read 64B
  uint32_t reserved0;      // 04800008..0480000B SI: Reserved
  uint32_t reserved1;      // 0480000C..0480000F SI: Reserved
  uint32_t PIF_ADDR_WR64B; // 04800010..04800013 SI: Address Write 64B
  uint32_t reserved2;      // 04800014..04800017 SI: Reserved
  uint32_t STATUS;         // 04800018..0480001B SI: Status
} SI;

extern const volatile struct {
  uint8_t RAM[0x0007FFFF]; // 05000000..0507FFFF Cartridge Domain 2 (Address 1) SRAM
} CART_DOM2_ADDR1;

extern const volatile struct {
  uint8_t RAM[0x01FFFFFF]; // 06000000..07FFFFFF Cartridge Domain 1 (Address 1) 64DD
} CART_DOM1_ADDR1;

extern const volatile struct {
  uint8_t RAM[0x07FFFFFF]; // 08000000..0FFFFFFF Cartridge Domain 2 (Address 2) SRAM
} CART_DOM2_ADDR2;

extern const volatile struct {
  uint8_t ROM[0x08000803]; // 10000000..18000803 Cartridge Domain 1 (Address 2) ROM
} CART_DOM1_ADDR2;

extern const volatile struct {
  uint8_t ROM[0x7C0]; // 1FC00000..1FC007BF PIF: Boot ROM
  uint8_t RAM[0x40];  // 1FC007C0..1FC007FF PIF: RAM (JoyChannel)
} PIF;

extern const volatile struct {
  uint8_t RAM[0x602FFFFF]; // 1FD00000..7FFFFFFF Cartridge Domain 1 (Address 3)
} CART_DOM1_ADDR3;

extern const volatile struct {
  uint8_t RAM[0x7FFFFFFF]; // 80000000..FFFFFFFF External SysAD Device
} EXT_SYS_AD;