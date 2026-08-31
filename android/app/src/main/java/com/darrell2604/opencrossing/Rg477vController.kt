package com.darrell2604.opencrossing

/** Neutral controller state consumed by the native game layer. */
data class ControllerState(
    val a: Boolean = false,
    val b: Boolean = false,
    val x: Boolean = false,
    val y: Boolean = false,
    val start: Boolean = false,
    val dpadUp: Boolean = false,
    val dpadDown: Boolean = false,
    val dpadLeft: Boolean = false,
    val dpadRight: Boolean = false,
    val leftX: Float = 0f,
    val leftY: Float = 0f,
    val rightX: Float = 0f,
    val rightY: Float = 0f,
    val leftTrigger: Float = 0f,
    val rightTrigger: Float = 0f
)

/** RG477V profile. Hardware events are translated here, not in game logic. */
object Rg477vController {
    fun normaliseAxis(value: Float): Float = value.coerceIn(-1f, 1f)

    fun applyDeadZone(value: Float, deadZone: Float = 0.12f): Float {
        val v = normaliseAxis(value)
        if (kotlin.math.abs(v) <= deadZone) return 0f
        val sign = if (v < 0f) -1f else 1f
        return sign * ((kotlin.math.abs(v) - deadZone) / (1f - deadZone))
    }
}
