#pragma once

#include <QtCore/QObject>
#include "settings.h"

class Application: public QObject
{
    public:
        static Application& getInstance() {
            static Application instance;
            return instance;
        }

        QString name() const;

        const Settings& settings() const {
            return settings_;
        }
        Settings& settings() {
            return settings_;
        }

    private slots:
        void quitting();

    private:
        Application();

        Settings settings_;
};

Application& app();
