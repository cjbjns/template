#include "core_base.h"

/**
 * \brief  Enable ICache
 * \details
 * This function enable I-Cache
 * \remarks
 * - This function can be called in M-Mode only.
 * - This \ref CSR_MCACHE_CTL register control I Cache enable.
 * \sa
 * - \ref DisableICache
*/
__STATIC_FORCEINLINE void EnableICache(void)
{
    __RV_CSR_SET(CSR_MCACHE_CTL, CSR_MCACHE_CTL_IE);
}

/**
 * \brief  Disable ICache
 * \details
 * This function Disable I-Cache
 * \remarks
 * - This function can be called in M-Mode only.
 * - This \ref CSR_MCACHE_CTL register control I Cache enable.
 * \sa
 * - \ref EnableICache
 */
__STATIC_FORCEINLINE void DisableICache(void)
{
    __RV_CSR_CLEAR(CSR_MCACHE_CTL, CSR_MCACHE_CTL_IE);
}

