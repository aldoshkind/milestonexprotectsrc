/*
 * Milestone:
 * This file was generated by GStreamer's make_template script.
 * This element expects the incoming buffers to be video frames in XProtect format.
 * It will remove the first 32 bytes of the buffer, which is the XProtect Generic 
 * Byte Data Header.
 * The element has 2 source pads:
 * - src_video:  The buffer without the XProtect header will go into this pad.
 * - src_meta:   An empty buffer with an xprotectmeta structure will go into this pad, 
 *               where the xprotectmeta structure will contain the relevant metadata 
 *               information from the XProtect header. See the gstvpsxprotectmeta for 
 *               more information on this.
 *
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2019  <<user@hostname.org>>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

 /**
  * SECTION:element-fromxprotectconverter
  *
  * The element fromxprotectconverter strips off the XProtect Generic Byte Data Header
  * from the video frames.
  *
  * <refsect2>
  * <title>Example launch line</title>
  * |[
  * gst-launch -v -m fakesrc ! fromxprotectconverter ! fakesink
  * ]|
  * </refsect2>
  */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <inttypes.h>
#include <stdio.h>
#include <gst/gst.h>
#include "GenericByteData.h"
#include "gstvpsfromxprotectconverter.h"
#include "gstvpsxprotect.h"

  /* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
};

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE("sink",
  GST_PAD_SINK,
  GST_PAD_ALWAYS,
  GST_STATIC_CAPS("application/x-genericbytedata-octet-stream")
);

static GstStaticPadTemplate src_video_factory_h264 = 
GST_STATIC_PAD_TEMPLATE(
  "src_h264",
  GST_PAD_SRC,
  GST_PAD_SOMETIMES,
  GST_STATIC_CAPS ("video/x-h264")
);

static GstStaticPadTemplate src_video_factory_h265 = 
GST_STATIC_PAD_TEMPLATE(
  "src_h265",
  GST_PAD_SRC,
  GST_PAD_SOMETIMES,
  GST_STATIC_CAPS ("video/x-h265")
);

static GstStaticPadTemplate src_video_factory_jpeg = 
GST_STATIC_PAD_TEMPLATE(
  "src_jpeg",
  GST_PAD_SRC,
  GST_PAD_SOMETIMES,
  GST_STATIC_CAPS ("image/jpeg")
);


#define gst_fromxprotectconverter_parent_class parent_class
G_DEFINE_TYPE(GstFromXprotectConverter, gst_fromxprotectconverter, GST_TYPE_BIN);

static gboolean gst_fromxprotectconverter_sink_event(GstPad * pad, GstObject * parent, GstEvent * event);
static gboolean gst_fromxprotectconverter_src_event(GstPad * pad, GstObject * parent, GstEvent * event);
static GstFlowReturn gst_fromxprotectconverter_chain(GstPad * pad, GstObject * parent, GstBuffer * buf);

/* GObject vmethod implementations */

/* initialize the fromxprotectconverter's class */
static void
gst_fromxprotectconverter_class_init(GstFromXprotectConverterClass * klass)
{
  GST_DEBUG_CATEGORY_INIT(gst_xprotect_debug, "xprotect",
    0, "Template fromxprotectconverter");
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *)klass;
  gstelement_class = (GstElementClass *)klass;

  gst_element_class_set_details_simple(gstelement_class,
    "fromxprotectconverter",
    "VPS/test",
    "Will remove a generic bytedata header from the frame.",
    "developer.milestonesys.com");

  gst_element_class_add_pad_template(gstelement_class,
    gst_static_pad_template_get(&src_video_factory_h264));
  gst_element_class_add_pad_template(gstelement_class,
    gst_static_pad_template_get(&src_video_factory_h265));
  gst_element_class_add_pad_template(gstelement_class,
    gst_static_pad_template_get(&src_video_factory_jpeg));

  gst_element_class_add_pad_template(gstelement_class,
    gst_static_pad_template_get(&sink_factory));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void gst_fromxprotectconverter_init(GstFromXprotectConverter * filter)
{
  if (filter == NULL)
  {
    GST_ERROR("Filter parameter was NULL.");
    return;
  }
  filter->sinkpad = gst_pad_new_from_static_template(&sink_factory, "sink");
  gst_pad_set_event_function(filter->sinkpad,
    GST_DEBUG_FUNCPTR(gst_fromxprotectconverter_sink_event));
  gst_pad_set_chain_function(filter->sinkpad,
    GST_DEBUG_FUNCPTR(gst_fromxprotectconverter_chain));
  // GST_PAD_SET_PROXY_CAPS(filter->sinkpad);
  gst_pad_use_fixed_caps(filter->sinkpad);
  gst_element_add_pad(GST_ELEMENT(filter), filter->sinkpad);

  filter->firstrun = TRUE;
}

/* this function handles sink events */
static gboolean
gst_fromxprotectconverter_sink_event(GstPad * pad, GstObject * parent, GstEvent * event)
{
  GstFromXprotectConverter *filter;
  gboolean ret;

  filter = GST_FROMXPROTECTCONVERTER(parent);

  GST_LOG_OBJECT(filter, "Received %s event: %" GST_PTR_FORMAT,
    GST_EVENT_TYPE_NAME(event), event);

  switch (GST_EVENT_TYPE(event)) {
  case GST_EVENT_CAPS:
  {
    // GstCaps * caps;

    // gst_event_parse_caps(event, &caps);
    /* do something with the caps */

    /* and forward */
    // ret = gst_pad_event_default(pad, parent, event);
    gst_event_unref(event);
    ret = TRUE;
    break;
  }
  case GST_EVENT_RECONFIGURE:
  {
    gst_event_unref(event);
    ret = TRUE;
    break;
  }
  default:
    ret = gst_pad_event_default(pad, parent, event);
    break;
  }
  return ret;
}

/* chain function
 * this function does the actual processing
 */
static GstFlowReturn gst_fromxprotectconverter_chain(GstPad * pad, GstObject * parent, GstBuffer * buf)
{
  GstFromXprotectConverter *filter;

  filter = GST_FROMXPROTECTCONVERTER(parent);

  GstMapInfo info;
  gst_buffer_map(buf, &info, GST_MAP_READ);

  VpsUtilities::GenericByteData * gbd = new VpsUtilities::GenericByteData((unsigned char*)info.data, (unsigned int) info.size, false, false);

  // TODO: Not sure how we should skip pushing a buffer?
  if (gbd->GetDataType() != VpsUtilities::DataType::VIDEO) {
    gst_buffer_unmap(buf, &info);
    gst_buffer_unref(buf);
    delete gbd;
    return GST_FLOW_OK;
  }

  if (filter->firstrun) {
    GstEvent *event;
    GstSegment segment;

    switch(gbd->GetCodec()) {
      case VpsUtilities::Codec::H264:
        filter->srcpad_video = gst_pad_new_from_static_template(&src_video_factory_h264, "src_h264");
        break;
      case VpsUtilities::Codec::H265:
        filter->srcpad_video = gst_pad_new_from_static_template(&src_video_factory_h265, "src_h265");
        break;
      case VpsUtilities::Codec::JPEG:
        filter->srcpad_video = gst_pad_new_from_static_template(&src_video_factory_jpeg, "src_jpeg");
        break;
      default:
        gst_buffer_unmap(buf, &info);
        gst_buffer_unref(buf);
        delete gbd;
        return GST_FLOW_NOT_SUPPORTED;
    }

    gst_pad_use_fixed_caps(filter->srcpad_video);
    gst_pad_set_active (filter->srcpad_video, TRUE);
    gst_element_add_pad(GST_ELEMENT(filter), filter->srcpad_video);

    // Send events to tell the rest of the pipeline we're configured and ready to go
    gst_pad_push_event (filter->srcpad_video, gst_event_new_stream_start ("src"));
    gst_segment_init (&segment, GST_FORMAT_BYTES);
    gst_pad_push_event (filter->srcpad_video, gst_event_new_segment (&segment));

    filter->firstrun = FALSE;
  }

  GstBuffer * outputBuffer = gst_buffer_new();
  GstMemory * mem = gst_allocator_alloc(NULL, gbd->GetBodyLength(), NULL);
  gst_buffer_append_memory(outputBuffer, mem);
  GstMapInfo info2;
  gst_buffer_map(outputBuffer, &info2, GST_MAP_WRITE);

  GST_TRACE_OBJECT(gst_xprotect_debug, "FROM seq no: %u\n", gbd->GetSequenceNumber());
  GST_TRACE_OBJECT(gst_xprotect_debug, "FROM Sync ts no: %" PRIu64 "\n", gbd->GetSyncTimeStamp());
  GST_TRACE_OBJECT(gst_xprotect_debug, "FROM ts no: %" PRIu64 "\n", gbd->GetTimeStamp());

  GST_BUFFER_DTS(outputBuffer) = GST_BUFFER_DTS(buf);
  GST_BUFFER_PTS(outputBuffer) = GST_BUFFER_PTS(buf);
  GST_BUFFER_OFFSET(outputBuffer) = GST_BUFFER_OFFSET(buf);

  // Theoretically, we can just modify the buffer in-place
  // TODO: Look into this
  memcpy(info2.data, gbd->GetBody(), gbd->GetBodyLength());

  delete gbd;

  gst_buffer_unmap(outputBuffer, &info2);

  gst_buffer_unmap(buf, &info);
  gst_buffer_unref(buf);
  
  return gst_pad_push(filter->srcpad_video, outputBuffer);
}
