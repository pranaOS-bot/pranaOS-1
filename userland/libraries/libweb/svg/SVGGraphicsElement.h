/*
 * Copyright (c) 2021, Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

// includes
#include <libgfx/Path.h>
#include <libweb/dom/Node.h>
#include <libweb/svg/SVGElement.h>
#include <libweb/svg/TagNames.h>

namespace Web::SVG {

class SVGGraphicsElement : public SVGElement {
public:
    using WrapperType = Bindings::SVGGraphicsElementWrapper;

    SVGGraphicsElement(DOM::Document&, QualifiedName);

    virtual void parse_attribute(const FlyString& name, const String& value) override;

    const Optional<Gfx::Color>& fill_color() const { return m_fill_color; }
    const Optional<Gfx::Color>& stroke_color() const { return m_stroke_color; }
    const Optional<float>& stroke_width() const { return m_stroke_width; }

protected:
    Optional<Gfx::Color> m_fill_color;
    Optional<Gfx::Color> m_stroke_color;
    Optional<float> m_stroke_width;
};

}
