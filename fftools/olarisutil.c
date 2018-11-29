/*
 * Copyright (c) 2016 Plex, Inc.
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "olarisutil.h"

#include "libavformat/avio.h"
#include "libavutil/dict.h"
#include "libavutil/avstring.h"

//#define DEBUG 1

char* HTTP_USER_AGENT = 0;
char* HTTP_COOKIES = 0;

void ff_http_set_userAgent(const char* myAgent)
{
  HTTP_USER_AGENT = (char* )myAgent;
}

void ff_http_set_cookie(const char* myCookie)
{
  HTTP_COOKIES = (char* )myCookie;
}

char* olaris_issue_http_request(const char* url, const char* verb)
{
    char* reply = NULL;
    AVIOContext *ioctx = NULL;
    AVDictionary *settings = NULL;
    int size = 0;
    int ret;

    av_dict_set(&settings, "user_agent", HTTP_USER_AGENT, 0);
    av_dict_set(&settings, "cookies", HTTP_COOKIES, 0);
    av_dict_set(&settings, "method", verb, 0);
    av_dict_set(&settings, "timeout", "1000000", 0);

    ret = avio_open2(&ioctx, url, AVIO_FLAG_READ,
                     NULL,
                     &settings);

    if (ret < 0)
        goto fail;

    size = avio_size(ioctx);
    if (size < 0)
        size = 4096;
    else if (!size)
        goto fail;

    reply = av_malloc(size);

    ret = avio_read(ioctx, reply, size);

    if (ret < 0)
        *reply = 0;

    avio_close(ioctx);
    av_dict_free(&settings);
    return reply;

fail:
    avio_close(ioctx);
    av_dict_free(&settings);
    reply = av_malloc(1);
    *reply = 0;
    return reply;
}

int olaris_opt_feedback_url(void *optctx, const char *opt, const char *arg) {
    olaris_feedback_url = (char *) arg;
    return 0;
}
