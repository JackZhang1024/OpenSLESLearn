package com.luckyboy.opensleslearn

import android.content.Context
import android.util.Log
import java.io.File
import java.io.FileOutputStream
import java.io.InputStream

class AssetManagerUtil {

    companion object {
        private const val TAG = "AssetManagerUtil"

        @Volatile
        private var assetManagerUtils: AssetManagerUtil? = null

        fun getInstance() = assetManagerUtils ?: synchronized(this) {
            assetManagerUtils ?: AssetManagerUtil().also {
                assetManagerUtils = it
            }
        }
    }

    fun loadAssetsToFiles(context: Context, assetFileName:String, targetFileName:String, dirName:String ="video"): String {
        var inputStream: InputStream? = null
        var fos: FileOutputStream? = null
        var targetFilePath = ""
        try {
            val dir = context.getDir(dirName, Context.MODE_PRIVATE)
            val targetFile = File(dir, targetFileName)
            if (targetFile.exists()) {
                targetFile.delete()
            }
            inputStream = context.assets.open(assetFileName)
            // 拷贝到 // data/data/packagename/plugin目录下
            fos = FileOutputStream(targetFile)
            // 文件读取流
            val buffer = ByteArray(1024)
            var length = 0
            while (inputStream.read(buffer).also { length = it } != -1) {
                fos.write(buffer, 0, length)
            }
            targetFilePath = targetFile.absolutePath
        } catch (e: Exception) {
            e.printStackTrace()
            Log.d(TAG, "loadAssetsToFiles: ${e.message}")
        } finally {
            if (inputStream != null) {
                try {
                    inputStream.close()
                } catch (e: Exception) {
                    e.printStackTrace()
                }
            }
            if (fos != null) {
                try {
                    fos.close()
                } catch (e: Exception) {
                    e.printStackTrace()
                }
            }
        }
        return targetFilePath
    }
}