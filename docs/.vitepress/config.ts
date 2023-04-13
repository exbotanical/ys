import { defineConfig } from 'vitepress'

// https://vitepress.dev/reference/site-config
export default defineConfig({
  base: '/ys/',
  lang: 'en-US',
  title: 'Ys',
  description:
    'Minimal web application framework for the C programming language',
  lastUpdated: true,
  cleanUrls: true,

  themeConfig: {
    nav: nav(),

    sidebar: {
      '/': sidebarMain(),
      '/reference/': sidebarReference(),
    },

    socialLinks: [
      { icon: 'github', link: 'https://github.com/exbotanical/ys' },
    ],
  },
  markdown: {
    theme: 'rose-pine-moon',
  },
})

function nav() {
  return [
    { text: 'Home', link: '/' },
    { text: 'Examples', link: 'https://github.com/exbotanical/ys/examples' },
  ]
}

function sidebarMain() {
  return [
    {
      text: 'Introduction',
      collapsed: false,
      items: [
        { text: 'What is Ys?', link: '/guide/what-is-ys' },
        { text: 'Platform Support', link: '/guide/platform-support' },
        { text: 'Getting Started', link: '/guide/getting-started' },
        { text: 'Basic Tutorial', link: '/guide/basic-tutorial' },
      ],
    },

    {
      text: 'Documentation',
      collapsed: false,
      items: [
        { text: 'Routing and Middleware', link: '/documentation/routing' },
        {
          text: 'HTTPs Support',
          link: '/documentation/https-support',
        },
        {
          text: 'CORS',
          link: '/documentation/cors',
        },
        {
          text: 'Cookies',
          link: '/documentation/cookies',
        },
        {
          text: 'Configuration and Logging',
          link: '/documentation/configuration-and-logging',
        },
      ],
    },

    {
      text: 'API Reference',
      link: '/reference/',
    },
  ]
}

function sidebarReference() {
  return [
    {
      text: 'API Reference',
      link: '/reference/',
      items: [
        {
          text: 'Constants',
          link: '/reference/constants',
        },
        {
          text: 'Config',
          link: '/reference/config',
        },
        {
          text: 'Request',
          link: '/reference/request',
        },
        {
          text: 'Response',
          link: '/reference/response',
        },
        {
          text: 'Router',
          link: '/reference/router',
        },
        {
          text: 'Router Attributes',
          link: '/reference/router-attr',
        },
        {
          text: 'Route Handler',
          link: '/reference/route-handler',
        },
        {
          text: 'Server',
          link: '/reference/server',
        },
        {
          text: 'Middleware',
          link: '/reference/middleware',
        },
        {
          text: 'CORS',
          link: '/reference/cors',
        },
        {
          text: 'Cookies',
          link: '/reference/cookies',
        },
      ],
    },
  ]
}
