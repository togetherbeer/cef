// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef CEF_LIBCEF_BROWSER_PERMISSION_CONTROLLER_DELEGATE_H
#define CEF_LIBCEF_BROWSER_PERMISSION_CONTROLLER_DELEGATE_H

#include "base/callback_forward.h"
#include "base/macros.h"
#include "content/public/browser/permission_controller_delegate.h"

class CefPermissionControllerDelegate : public content::PermissionControllerDelegate {
public:
  CefPermissionControllerDelegate();
  ~CefPermissionControllerDelegate() override;

  // PermissionControllerDelegate methods
  int RequestPermission(content::PermissionType permission,
                        content::RenderFrameHost *render_frame_host,
                        const GURL &requesting_origin,
                        bool user_gesture,
                        base::OnceCallback<void(blink::mojom::PermissionStatus)>
                            callback) override;

  int RequestPermissions(
      const std::vector<content::PermissionType> &permission,
      content::RenderFrameHost *render_frame_host,
      const GURL &requesting_origin,
      bool user_gesture,
      base::OnceCallback<
          void(const std::vector<blink::mojom::PermissionStatus> &)>
          callback)
      override;

  void ResetPermission(content::PermissionType permission,
                       const GURL &requesting_origin,
                       const GURL &embedding_origin) override;

  blink::mojom::PermissionStatus GetPermissionStatus(
      content::PermissionType permission,
      const GURL &requesting_origin,
      const GURL &embedding_origin) override;

  blink::mojom::PermissionStatus GetPermissionStatusForFrame(
      content::PermissionType permission,
      content::RenderFrameHost *render_frame_host,
      const GURL &requesting_origin) override;

  int SubscribePermissionStatusChange(
      content::PermissionType permission,
      content::RenderFrameHost *render_frame_host,
      const GURL &requesting_origin,
      base::RepeatingCallback<void(blink::mojom::PermissionStatus)> callback)
      override;

  void UnsubscribePermissionStatusChange(int subscription_id) override;

private:
  DISALLOW_COPY_AND_ASSIGN(CefPermissionControllerDelegate);
};

#endif // CEF_LIBCEF_BROWSER_PERMISSION_CONTROLLER_DELEGATE_H
