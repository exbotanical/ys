<script setup lang="ts">
// Necessary to use this wrapper to ensure the noise effect runs client-side only
import { onMounted } from 'vue'
import DefaultTheme from 'vitepress/theme'
const { Layout } = DefaultTheme

onMounted(() => {
  initNoiseEffect().then(png => {
    if (!png) return

    const url = URL.createObjectURL(png)
    const div = document.createElement('div')

    div.classList.add('noise')
    ;(
      div as unknown as { style: string }
    ).style = `background-image: url(${url})`

    document.body.appendChild(div)
  })
  function initNoiseEffect() {
    const size = 64
    const black = new Uint8ClampedArray([0, 0, 0, 255])
    const white = new Uint8ClampedArray([255, 255, 255, 255])
    const data = new Uint8ClampedArray(size * size * 4)

    for (let y = 0; y < size; y++) {
      for (let x = 0; x < size; x++) {
        data.set(Math.random() > 0.5 ? white : black, (y * size + x) * 4)
      }
    }

    const canvas = document.createElement('canvas')
    canvas.width = canvas.height = size

    const ctx = canvas.getContext('2d')
    ctx?.putImageData(new ImageData(data, size, size), 0, 0)

    return new Promise<Blob | null>((resolve, reject) => {
      canvas.toBlob(b => (b ? resolve(b) : reject()), 'image/png')
    })
  }
})
</script>

<template>
  <Layout />
</template>
