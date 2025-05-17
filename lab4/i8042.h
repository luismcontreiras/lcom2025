#ifndef _I8042_H_
#define _I8042_H_

#include <lcom/lcf.h>

#define IRQ12 12

/* I/O port addresses */
#define KBC_ST_REG   0x64   // KBC status register :contentReference[oaicite:0]{index=0}:contentReference[oaicite:1]{index=1}
#define KBC_CMD_REG      0x64   // KBC command register :contentReference[oaicite:2]{index=2}:contentReference[oaicite:3]{index=3}
#define KBC_OUT_BUF      0x60   // KBC output buffer (data port) :contentReference[oaicite:4]{index=4}:contentReference[oaicite:5]{index=5}
#define KBC_IN_BUF       0x60   // KBC input buffer (data port) :contentReference[oaicite:6]{index=6}:contentReference[oaicite:7]{index=7}  // KBC status register :contentReference[oaicite:8]{index=8}:contentReference[oaicite:9]{index=9}
/* Status‐register bit masks */
#define KBC_PARITY_ERR   BIT(7)  // Parity error :contentReference[oaicite:8]{index=8}:contentReference[oaicite:9]{index=9}
#define KBC_TIMEOUT_ERR  BIT(6)  // Timeout error :contentReference[oaicite:10]{index=10}:contentReference[oaicite:11]{index=11}
#define KBC_AUX          BIT(5)  // Mouse data :contentReference[oaicite:12]{index=12}:contentReference[oaicite:13]{index=13}
#define KBC_INHIBIT      BIT(4)  // Inhibit flag :contentReference[oaicite:14]{index=14}:contentReference[oaicite:15]{index=15}
#define KBC_A2           BIT(3)  // A2 input line :contentReference[oaicite:16]{index=16}:contentReference[oaicite:17]{index=17}
#define KBC_SYS          BIT(2)  // System flag :contentReference[oaicite:18]{index=18}:contentReference[oaicite:19]{index=19}
#define KBC_IBF          BIT(1)  // Input buffer full :contentReference[oaicite:20]{index=20}:contentReference[oaicite:21]{index=21}
#define KBC_OBF          BIT(0)  // Output buffer full :contentReference[oaicite:22]{index=22}:contentReference[oaicite:23]{index=23}

/* KBC “Command Byte” commands */
#define KBC_READ_CMD     0x20    // Read command byte :contentReference[oaicite:24]{index=24}:contentReference[oaicite:25]{index=25}
#define KBC_WRITE_CMD    0x60    // Write command byte :contentReference[oaicite:26]{index=26}:contentReference[oaicite:27]{index=27}

/* PS/2‐mouse‐related KBC commands (write to 0x64, data via 0x60) */
#define KBC_DISABLE_MOUSE 0xA7   // Disable mouse :contentReference[oaicite:28]{index=28}:contentReference[oaicite:29]{index=29}
#define KBC_ENABLE_MOUSE  0xA8   // Enable mouse :contentReference[oaicite:30]{index=30}:contentReference[oaicite:31]{index=31}
#define KBC_CHECK_MOUSE   0xA9   // Check mouse interface :contentReference[oaicite:32]{index=32}:contentReference[oaicite:33]{index=33}
#define KBC_WRITE_MOUSE   0xD4   // Forward next byte to mouse :contentReference[oaicite:34]{index=34}:contentReference[oaicite:35]{index=35}

/* Mouse ACK/NACK/Error codes (returned in OUT_BUF) */
#define MOUSE_ACK         0xFA   // OK :contentReference[oaicite:36]{index=36}:contentReference[oaicite:37]{index=37}
#define MOUSE_NACK        0xFE   // Resend request :contentReference[oaicite:38]{index=38}:contentReference[oaicite:39]{index=39}
#define MOUSE_ERROR       0xFC   // Error :contentReference[oaicite:40]{index=40}:contentReference[oaicite:41]{index=41}

/* Delay for polling IBF/OBF without busy‐waiting */
#define DELAY_US          20000  // 20 ms :contentReference[oaicite:42]{index=42}:contentReference[oaicite:43]{index=43}

#endif /* _I8042_H_ */
