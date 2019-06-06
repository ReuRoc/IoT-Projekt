#!/bin/sh
pm2 start test_provider/provider_test_color.js
node test_provider/pm2_provider.js
