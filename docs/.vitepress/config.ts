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
      { icon: 'github', link: 'https://github.com/exbotanical/libys' },
    ],
  },
})

function nav() {
  return [
    { text: 'Home', link: '/' },
    { text: 'Recipes', link: '/recipes/' },
  ]
}

function sidebarMain() {
  return [
    {
      text: 'Introduction',
      collapsed: false,
      items: [
        { text: 'What is Ys?', link: '/guide/what-is-ys' },
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
      text: 'Recipes',
      collapsed: false,
      items: [
        { text: 'Simple Server', link: '/recipes/simple-recipe' },
        { text: 'Authentication', link: '/recipes/auth-recipe' },
        {
          text: 'Custom Configurations',
          link: '/recipes/custom-config-recipe',
        },
        { text: 'CORS Server', link: '/recipes/cors-recipe' },
        { text: 'Middleware', link: '/recipes/middleware-recipe' },
        { text: 'Nested Routing', link: '/recipes/nested-routing-recipe' },
        { text: 'REST Server', link: '/recipes/rest-recipe' },
        { text: 'HTTPs Server', link: '/recipes/https-recipe' },
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
