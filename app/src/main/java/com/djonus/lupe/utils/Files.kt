package com.djonus.lupe.utils

import android.content.Context
import android.util.Log
import java.io.File
import java.io.FileInputStream
import java.io.FileOutputStream
import java.nio.ByteBuffer

suspend fun saveToFile(data: FloatArray, file: File) {
    log("File save: $file")
    try {
        val out = FileOutputStream(file)
        data.forEachIndexed { index, sample ->
            val bytes = ByteBuffer.allocate(4).putFloat(sample).array()
            out.write(bytes)
        }
        out.flush()
        out.close()
        log("File saved: $file")
    } catch (e: Exception) {
        e.printStackTrace()
        log("File save failed: $file $e")
    }
}

suspend fun loadFromFile(file: File): FloatArray {
    log("File load start: $file")
    val data = mutableListOf<Float>()
    try {
        val ins = FileInputStream(file)
        val buffer = ByteArray(4)

        var byte = ins.read(buffer)
        while (byte != -1) {
            data.add(ByteBuffer.wrap(buffer).float)
            byte = ins.read(buffer)
        }
        log("File load success: $file\n float sample count: ${data.size}")
        ins.close()
    } catch (e: Exception) {
        e.printStackTrace()
        log("File load error: $file $e")
    }
    return data.toFloatArray()
}

fun Context.loopsDir(): File {
    val root = getExternalFilesDir(null)
    return File(root, "/LupeLoops")
}

private fun log(msg: String) {
    Log.d("LUPE", msg)
}
