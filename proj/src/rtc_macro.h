#ifndef RTC_MACRO
#define RTC_MACRO



/** @defgroup RTC_MACRO RTC_MACRO
 * @{
 *
 * Symbolic constants used to work with the real time clock.
 */

#ifndef BIT
#define BIT(n)                (0x01<<(n))
#endif

#define RTC_IRQ               8         /**< @brief Interrupt request line used by the RTC */
#define RTC_ADDR_REG       0x70         /**< @brief Address-selecting register */
#define RTC_DATA_REG       0x71         /**< @brief Data Input/output register */

#define RTC_SEC            0x00         /**< @brief Seconds register */
#define RTC_SEC_ALARM      0x01         /**< @brief Seconds alarm register */
#define RTC_MIN            0x02         /**< @brief Minutes register */
#define RTC_MIN_ALARM      0x03         /**< @brief Minutes alarm register */
#define RTC_HOUR           0x04         /**< @brief Hours register */
#define RTC_HOUR_ALARM     0x05         /**< @brief Hours alarm register */
#define RTC_DOW            0x06         /**< @brief Day of the week register */
#define RTC_DOM            0x07         /**< @brief Day of the month register */
#define RTC_MON            0x08         /**< @brief Month register */
#define RTC_YEAR           0x09         /**< @brief Year register */

#define RTC_REG_A          0x0A         /**< @brief RTC's A Register */
#define RTC_REG_B          0x0B         /**< @brief RTC's B Register */
#define RTC_REG_C          0x0C         /**< @brief RTC's C Register */
#define RTC_REG_D          0x0D         /**< @brief RTC's D Register */

#define RTC_UIP            BIT(7)       /**< @brief Update in progress flag */

#define RTC_DC_ALARM       0xC0         /**< @brief Don't care alarm bit */

#define RTC_SET            BIT(7)       /**< @brief Enable updates bit */
#define RTC_PIE            BIT(6)       /**< @brief Periodic interrupts Enable bit */
#define RTC_AIE            BIT(5)       /**< @brief Periodic interrupts Enable bit */
#define RTC_UIE            BIT(4)       /**< @brief Update interrupts Enable bit */
#define RTC_DM             BIT(2)       /**< @brief Value format bit(BCD or Binary) */

#define RTC_PI_PENDING     BIT(6)       /**< @brief Periodic interrupt pending bit */
#define RTC_AI_PENDING     BIT(5)       /**< @brief Alarm interrupt pending bit */
#define RTC_UI_PENDING     BIT(4)       /**< @brief Update interrupt pending bit */

#endif
