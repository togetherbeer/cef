// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/renderer/url_loader_throttle_provider_impl.h"

#include "libcef/common/extensions/extensions_util.h"
#include "libcef/renderer/render_thread_observer.h"

#include <utility>

#include "base/feature_list.h"
#include "chrome/common/google_url_loader_throttle.h"
#include "content/public/common/content_features.h"
#include "content/public/renderer/render_frame.h"
#include "extensions/renderer/guest_view/mime_handler_view/mime_handler_view_container.h"
#include "services/network/public/cpp/features.h"
#include "third_party/blink/public/common/loader/resource_type_util.h"
#include "third_party/blink/public/platform/web_url.h"

CefURLLoaderThrottleProviderImpl::CefURLLoaderThrottleProviderImpl(
    content::URLLoaderThrottleProviderType type)
    : type_(type) {
  DETACH_FROM_THREAD(thread_checker_);
}

CefURLLoaderThrottleProviderImpl::~CefURLLoaderThrottleProviderImpl() {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
}

CefURLLoaderThrottleProviderImpl::CefURLLoaderThrottleProviderImpl(
    const CefURLLoaderThrottleProviderImpl& other)
    : type_(other.type_) {
  DETACH_FROM_THREAD(thread_checker_);
}

std::unique_ptr<content::URLLoaderThrottleProvider>
CefURLLoaderThrottleProviderImpl::Clone() {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  return base::WrapUnique(new CefURLLoaderThrottleProviderImpl(*this));
}

std::vector<std::unique_ptr<blink::URLLoaderThrottle>>
CefURLLoaderThrottleProviderImpl::CreateThrottles(
    int render_frame_id,
    const blink::WebURLRequest& request) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);

  std::vector<std::unique_ptr<blink::URLLoaderThrottle>> throttles;

  const network::mojom::RequestDestination request_destination =
      request.GetRequestDestination();

  // Some throttles have already been added in the browser for frame resources.
  // Don't add them for frame requests.
  bool is_frame_resource =
      blink::IsRequestDestinationFrame(request_destination);

  DCHECK(!is_frame_resource ||
         type_ == content::URLLoaderThrottleProviderType::kFrame);

  if (extensions::ExtensionsEnabled() &&
      type_ == content::URLLoaderThrottleProviderType::kFrame &&
      request_destination == network::mojom::RequestDestination::kObject) {
    content::RenderFrame* render_frame =
        content::RenderFrame::FromRoutingID(render_frame_id);
    auto mime_handlers =
        extensions::MimeHandlerViewContainer::FromRenderFrame(render_frame);
    GURL gurl(request.Url());
    for (auto* handler : mime_handlers) {
      auto throttle = handler->MaybeCreatePluginThrottle(gurl);
      if (throttle) {
        throttles.push_back(std::move(throttle));
        break;
      }
    }
  }

  throttles.push_back(std::make_unique<GoogleURLLoaderThrottle>(
      CefRenderThreadObserver::GetDynamicParams()));

  return throttles;
}

void CefURLLoaderThrottleProviderImpl::SetOnline(bool is_online) {}
