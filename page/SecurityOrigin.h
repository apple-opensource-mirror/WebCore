/*
 * Copyright (C) 2007,2008 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SecurityOrigin_h
#define SecurityOrigin_h

#include <wtf/HashSet.h>
#include <wtf/RefCounted.h>
#include <wtf/PassRefPtr.h>
#include <wtf/Threading.h>

#include "FrameLoaderTypes.h"
#include "PlatformString.h"
#include "StringHash.h"

namespace WebCore {

typedef HashSet<String, CaseFoldingHash> URLSchemesMap;

class Document;
class KURL;

class SecurityOrigin : public ThreadSafeShared<SecurityOrigin> {
public:
    static PassRefPtr<SecurityOrigin> createFromDatabaseIdentifier(const String&);
    static PassRefPtr<SecurityOrigin> createFromString(const String&);
    static PassRefPtr<SecurityOrigin> create(const KURL&, SandboxFlags = SandboxNone);
    static PassRefPtr<SecurityOrigin> createEmpty();

    // Create a deep copy of this SecurityOrigin. This method is useful
    // when marshalling a SecurityOrigin to another thread.
    PassRefPtr<SecurityOrigin> threadsafeCopy();

    // Set the domain property of this security origin to newDomain. This
    // function does not check whether newDomain is a suffix of the current
    // domain. The caller is responsible for validating newDomain.
    void setDomainFromDOM(const String& newDomain);
    bool domainWasSetInDOM() const { return m_domainWasSetInDOM; }

    static void setDomainRelaxationForbiddenForURLScheme(bool forbidden, const String&);
    static bool isDomainRelaxationForbiddenForURLScheme(const String&);

    String protocol() const { return m_protocol; }
    String host() const { return m_host; }
    String domain() const { return m_domain; }
    unsigned short port() const { return m_port; }

    // Returns true if this SecurityOrigin can script objects in the given
    // SecurityOrigin. For example, call this function before allowing
    // script from one security origin to read or write objects from
    // another SecurityOrigin.
    bool canAccess(const SecurityOrigin*) const;

    // Returns true if this SecurityOrigin can read content retrieved from
    // the given URL. For example, call this function before issuing
    // XMLHttpRequests.
    bool canRequest(const KURL&) const;

    // Returns true if drawing an image from this URL taints a canvas from
    // this security origin. For example, call this function before
    // drawing an image onto an HTML canvas element with the drawImage API.
    bool taintsCanvas(const KURL&) const;

    // Returns true for any non-local URL. If document parameter is supplied,
    // its local load policy dictates, otherwise if referrer is non-empty and
    // represents a local file, then the local load is allowed.
    static bool canLoad(const KURL&, const String& referrer, Document* document);

    // Returns true if this SecurityOrigin can load local resources, such
    // as images, iframes, and style sheets, and can link to local URLs.
    // For example, call this function before creating an iframe to a
    // file:// URL.
    //
    // Note: A SecurityOrigin might be allowed to load local resources
    //       without being able to issue an XMLHttpRequest for a local URL.
    //       To determine whether the SecurityOrigin can issue an
    //       XMLHttpRequest for a URL, call canRequest(url).
    bool canLoadLocalResources() const { return m_canLoadLocalResources; }

    // Explicitly grant the ability to load local resources to this
    // SecurityOrigin.
    //
    // Note: This method exists only to support backwards compatibility
    //       with older versions of WebKit.
    void grantLoadLocalResources();

    // Explicitly grant the ability to access very other SecurityOrigin.
    //
    // WARNING: This is an extremely powerful ability. Use with caution!
    void grantUniversalAccess();

    void setSandboxFlags(SandboxFlags);
    bool isSandboxed(SandboxFlags mask) const { return m_sandboxFlags & mask; }

    bool canAccessDatabase() const { return !isUnique(); }
    bool canAccessStorage() const { return !isUnique(); }
    bool canAccessCookies() const { return !isUnique(); }

    bool isSecureTransitionTo(const KURL&) const;

    // The local SecurityOrigin is the most privileged SecurityOrigin.
    // The local SecurityOrigin can script any document, navigate to local
    // resources, and can set arbitrary headers on XMLHttpRequests.
    bool isLocal() const;

    // The empty SecurityOrigin is the least privileged SecurityOrigin.
    bool isEmpty() const;

    // The origin is a globally unique identifier assigned when the Document is
    // created. http://www.whatwg.org/specs/web-apps/current-work/#sandboxOrigin
    //
    // There's a subtle difference between a unique origin and an origin that
    // has the SandboxOrigin flag set. The latter implies the former, and, in
    // addition, the SandboxOrigin flag is inherited by iframes.
    bool isUnique() const { return m_isUnique; }

    // Convert this SecurityOrigin into a string. The string
    // representation of a SecurityOrigin is similar to a URL, except it
    // lacks a path component. The string representation does not encode
    // the value of the SecurityOrigin's domain property.
    //
    // When using the string value, it's important to remember that it might be
    // "null". This happens when this SecurityOrigin is unique. For example,
    // this SecurityOrigin might have come from a sandboxed iframe, the
    // SecurityOrigin might be empty, or we might have explicitly decided that
    // we shouldTreatURLSchemeAsNoAccess.
    String toString() const;

    // Serialize the security origin to a string that could be used as part of
    // file names. This format should be used in storage APIs only.
    String databaseIdentifier() const;

    // This method checks for equality between SecurityOrigins, not whether
    // one origin can access another. It is used for hash table keys.
    // For access checks, use canAccess().
    // FIXME: If this method is really only useful for hash table keys, it
    // should be refactored into SecurityOriginHash.
    bool equal(const SecurityOrigin*) const;

    // This method checks for equality, ignoring the value of document.domain
    // (and whether it was set) but considering the host. It is used for postMessage.
    bool isSameSchemeHostPort(const SecurityOrigin*) const;

    static void registerURLSchemeAsLocal(const String&);
    static void removeURLSchemeRegisteredAsLocal(const String&);
    static const URLSchemesMap& localURLSchemes();
    static bool shouldTreatURLAsLocal(const String&);
    static bool shouldTreatURLSchemeAsLocal(const String&);

    static bool shouldHideReferrer(const KURL&, const String& referrer);

    enum LocalLoadPolicy {
        AllowLocalLoadsForAll, // No restriction on local loads.
        AllowLocalLoadsForLocalAndSubstituteData,
        AllowLocalLoadsForLocalOnly,
    };
    static void setLocalLoadPolicy(LocalLoadPolicy);
    static bool restrictAccessToLocal();
    static bool allowSubstituteDataAccessToLocal();

    static void registerURLSchemeAsNoAccess(const String&);
    static bool shouldTreatURLSchemeAsNoAccess(const String&);

    static void whiteListAccessFromOrigin(const SecurityOrigin& sourceOrigin, const String& destinationProtocol, const String& destinationDomains, bool allowDestinationSubdomains);
    static void resetOriginAccessWhiteLists();

private:
    SecurityOrigin(const KURL&, SandboxFlags);
    explicit SecurityOrigin(const SecurityOrigin*);

    SandboxFlags m_sandboxFlags;
    String m_protocol;
    String m_host;
    mutable String m_encodedHost;
    String m_domain;
    unsigned short m_port;
    bool m_isUnique;
    bool m_universalAccess;
    bool m_domainWasSetInDOM;
    bool m_canLoadLocalResources;
};

} // namespace WebCore

#endif // SecurityOrigin_h
