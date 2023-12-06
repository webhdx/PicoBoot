MEMORY
{
    /*
     * Shrink flash region to make space for the payload.
     * 1.5MiB is plenty, and probably bigger than the original BS2 which we are constrained by anyway.
     * This results in `warning: redeclaration of memory region `FLASH'`.
     * Safe to ignore.
     */
    FLASH(rx) : ORIGIN = 0x10000000, LENGTH = 512k
    PAYLOAD(r) : ORIGIN = ORIGIN(FLASH) + LENGTH(FLASH), LENGTH = 2048k - LENGTH(FLASH)
}

__payload = ORIGIN(PAYLOAD);
__payload_end = __payload + LENGTH(PAYLOAD);
