/*
 * Copyright (c) 2021, Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once
#include "GraphView.h"
#include <libfoundation/ProcessInfo.h>
#include <libui/App.h>
#include <libui/Button.h>
#include <libui/Label.h>
#include <libui/StackView.h>
#include <libui/View.h>
#include <libui/ViewController.h>
#include <libui/Window.h>
#include <memory>
#include <sys/types.h>

static char buf[256];

class ViewController : public UI::ViewController<UI::View> {
public:
    ViewController(UI::View& view)
        : UI::ViewController<UI::View>(view)
        , m_cpu_count(LFoundation::ProcessInfo::the().processor_count())
    {
    }
    virtual ~ViewController() = default;

    inline int cpu_count() const { return m_cpu_count; }

    void view_did_load() override
    {
        state.cpu_load.resize(cpu_count());
        state.cpu_old_user_time.resize(cpu_count());
        state.cpu_old_system_time.resize(cpu_count());
        state.cpu_old_idle_time.resize(cpu_count());

        view().set_background_color(LG::Color::LightSystemBackground);

        auto& cpu_label = view().add_subview<UI::Label>(LG::Rect(0, 0, 180, 16));
        auto& cpu_graphs_stackview = view().add_subview<UI::StackView>(LG::Rect(0, 0, 184, 100));
        cpu_graphs_stackview.set_distribution(UI::StackView::Distribution::FillEqually);
        cpu_graphs_stackview.set_spacing(10);

        view().add_constraint(UI::Constraint(cpu_label, UI::Constraint::Attribute::Left, UI::Constraint::Relation::Equal, UI::SafeArea::Left));
        view().add_constraint(UI::Constraint(cpu_label, UI::Constraint::Attribute::Top, UI::Constraint::Relation::Equal, UI::SafeArea::Top));

        view().add_constraint(UI::Constraint(cpu_graphs_stackview, UI::Constraint::Attribute::Left, UI::Constraint::Relation::Equal, UI::SafeArea::Left));
        view().add_constraint(UI::Constraint(cpu_graphs_stackview, UI::Constraint::Attribute::Right, UI::Constraint::Relation::Equal, UI::SafeArea::Right));
        view().add_constraint(UI::Constraint(cpu_graphs_stackview, UI::Constraint::Attribute::Top, UI::Constraint::Relation::Equal, cpu_label, UI::Constraint::Attribute::Bottom, 1, 8));
        view().add_constraint(UI::Constraint(cpu_graphs_stackview, UI::Constraint::Attribute::Bottom, UI::Constraint::Relation::Equal, UI::SafeArea::Bottom));

        for (int i = 0; i < cpu_count(); i++) {
            auto& graph = cpu_graphs_stackview.add_arranged_subview<GraphView>(200);
            cpu_graphs_stackview.add_constraint(UI::Constraint(graph, UI::Constraint::Attribute::Height, UI::Constraint::Relation::Equal, cpu_graphs_stackview, UI::Constraint::Attribute::Height, 1, 0));
            cpu_graphs.push_back(&graph);
        }

        view().set_needs_layout();

        UI::App::the().event_loop().add(LFoundation::Timer([&] {
            update_data();
            cpu_label.set_text(std::string("Load ") + std::to_string(state.cpu_load[0]) + "%");
            cpu_label.set_needs_display();
        },
            1000, LFoundation::Timer::Repeat));
    }

    int update_cpu_load()
    {
        int fd_proc_stat = open("/proc/stat", O_RDONLY);
        int offset = 0;
        read(fd_proc_stat, buf, sizeof(buf));

        for (int i = 0; i < cpu_count(); i++) {
            int user_time, system_time, idle_time;
            int num;
            offset += sscanf(buf + offset, "cpu%d %d 0 %d %d\n", &num, &user_time, &system_time, &idle_time);
            int diff_user_time = user_time - state.cpu_old_user_time[i];
            int diff_system_time = system_time - state.cpu_old_system_time[i];
            int diff_idle_time = idle_time - state.cpu_old_idle_time[i];
            state.cpu_old_user_time[i] = user_time;
            state.cpu_old_system_time[i] = system_time;
            state.cpu_old_idle_time[i] = idle_time;

            if (diff_user_time + diff_system_time + diff_idle_time == 0) {
                state.cpu_load[i] = 0;
            } else {
                state.cpu_load[i] = (diff_user_time + diff_system_time) * 100 / (diff_user_time + diff_system_time + diff_idle_time);
            }

            cpu_graphs[i]->add_new_value(state.cpu_load[i]);
        }

        close(fd_proc_stat);
        return 0;
    }

    void update_data()
    {
        update_cpu_load();
    }

private:
    int m_cpu_count;
    int fd_proc_stat;
    std::vector<GraphView*> cpu_graphs;

    struct State {
        std::vector<int> cpu_load;
        std::vector<int> cpu_old_user_time;
        std::vector<int> cpu_old_system_time;
        std::vector<int> cpu_old_idle_time;
    };
    State state;
};