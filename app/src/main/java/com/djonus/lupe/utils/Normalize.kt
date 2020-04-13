package com.djonus.lupe.utils

import kotlin.math.pow

internal fun ClosedRange<Double>.normalize(out: ClosedRange<Double>, value: Float): Double =
    (out.endInclusive - out.start) / (endInclusive - start) * (value - start) + start

internal fun Double.exponential(factor: Double, range: ClosedRange<Double>): Double =
    pow(factor) * (1/(range.endInclusive.pow(factor-1))) + range.start
