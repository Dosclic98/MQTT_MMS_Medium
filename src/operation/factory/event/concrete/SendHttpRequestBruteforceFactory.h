//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef OPERATION_FACTORY_EVENT_CONCRETE_SENDHTTPREQUESTBRUTEFORCEFACTORY_H_
#define OPERATION_FACTORY_EVENT_CONCRETE_SENDHTTPREQUESTBRUTEFORCEFACTORY_H_

#include "../EventOperationFactory.h"
#include "../../../../utils/factories/http/HttpMessageFactory.h"

namespace inet {

class SendHttpRequestBruteforceFactory : public EventOperationFactory {
public:
    virtual void build(omnetpp::cEvent* event) override;

    SendHttpRequestBruteforceFactory(IController* controller);
    virtual ~SendHttpRequestBruteforceFactory();
protected:
    HttpMessageFactory messageFactory;
    std::string username = "admin";

    std::vector<std::string> passwordVector = {
        "admin", "password", "123456", "letmein", "qwerty", "welcome", "trustno1", "secret", "default", "access",
        "user", "guest", "changeme", "passw0rd", "root", "1234", "login", "abcd1234", "password1", "superuser",
        "system", "master", "test", "admin123", "administrator", "manager", "service", "guest123", "backup", "temp",
        "support", "newuser", "public", "demo", "secure", "oracle", "postgres", "mysql", "webmaster", "network",
        "control", "office", "developer", "server", "database", "poweruser", "privileged", "operator", "general", "basic",
        "interface", "nopassword", "easy123", "apple", "orange", "banana", "summer", "winter", "spring", "autumn",
        "home", "remote", "dashboard", "client", "proxy", "resetme", "letmein123", "open", "unlock", "readonly",
        "adminadmin", "supervisor", "system123", "network123", "admin2023", "user2023", "serveradmin", "adminpanel", "router", "device",
        "module", "config", "default123", "test123", "testuser", "netadmin", "defaultpass", "adminlogin", "power", "login123",
        "security", "abc123", "testpass", "initial", "trial", "adminpass", "access123", "mypassword", "guestlogin", "entry",
        "modem", "vpnuser", "camera", "switch", "bridge", "firewall", "routeradmin", "guestpass", "publicaccess", "openaccess",
        "super", "guestuser", "adminuser", "simple", "internet", "local", "noaccess", "company", "partner", "external",
        "internal", "service123", "testing", "defaultpass123", "entry123", "router123", "password1234", "passwd", "pass123", "setup",
        "configuration", "member", "custom", "device123", "unit", "installer", "dummy", "installer123", "webadmin", "database123",
        "company123", "empty", "general123", "standard", "manager123", "project", "operator123", "info123", "secure123", "noreply",
        "readonly123", "email", "testme", "systemuser", "linux", "windows", "adminroot", "adminmanager", "siteadmin", "dbadmin",
        "netuser", "dba", "sqladmin", "sqluser", "readonlyuser", "admintest", "testaccount", "cloudadmin", "clouduser", "guestuser123",
        "defaultroot", "password2023", "temp123", "readonlyadmin", "readonlyuser123", "user1234", "initial123", "empty123", "loginpass", "pass1234",
        "securelogin", "mypassword123", "nopassword123", "accessroot", "passme", "adminme", "guestme", "webguest", "webuser", "guest1234",
        "proxyuser", "networkguest", "networkuser", "networkadmin123", "webapp", "webappuser", "webappadmin", "readonly2023", "test1234", "oracleadmin",
        "oracleuser", "postgresadmin", "postgresuser", "mysqladmin", "mysqluser", "mongoadmin", "mongouser", "dbuser", "dbguest", "secureadmin",
        "testadmin", "testing123", "webtest", "apitest", "apiuser", "adminapi", "dbapi", "cloudapi", "dbguest123", "adminsetup",
        "setup123", "admincontrol", "testaccount123", "manageradmin", "manage123", "resetpassword", "resetme123", "guestlogin123", "userlogin", "userlogin123",
        "modemadmin", "modemuser", "vpnadmin", "vpnlogin", "vpnlogin123", "openvpn", "bridgeadmin", "bridgeuser", "guestpassword", "adminpassword",
        "routerpassword", "switchadmin", "switchuser", "networkmanager", "networkadmin2023", "firewalladmin", "firewalluser", "webmanager", "webpassword", "networkaccess",
        "localadmin", "internetadmin", "localpassword", "internetpassword", "emailadmin", "emailuser", "emailpassword", "externaluser", "externaladmin", "internaladmin",
        "internaluser", "servicelogin", "backuplogin", "templogin", "supportlogin", "supportpassword", "supportadmin", "serviceadmin", "testingaccount", "testingaccount123",
        "defaultadmin", "defaultlogin", "defaultuser", "defaultpassword2023", "readonlyaccess", "readonlyadmin123", "readonlylogin", "readonlypassword", "useraccess", "accesslogin",
        "setupaccount", "setupadmin", "setupuser", "setupaccess", "configadmin", "configuser", "configpassword", "deviceaccess", "moduleadmin", "moduleuser",
        "modulesetup", "moduleconfig", "defaultmodule", "supermodule", "unitadmin", "unituser", "unitpassword", "unitconfig", "devicelock", "deviceunlock",
        "publicuser", "publicadmin", "publicpassword", "internalaccess", "externalaccess", "userguest", "useradmin", "userpassword", "guestpassword123", "adminpassword123",
        "rootadmin", "guestaccess", "guestaccess123", "public123", "publicaccess123", "adminpanel123", "readonlypanel", "readonlypanel123", "webaccess", "vpnpassword",
        "readonlymodem", "readwrite", "readonlycloud", "readwritecloud", "readonlysql", "readwritesql", "userpanel", "userpanel123", "paneladmin", "paneluser",
        "panelpassword", "panelguest", "guestpanel", "guestpanel123", "userpanelaccess", "guestpanelaccess", "systempanel", "systempanel123", "loginadmin", "loginpanel",
        "routeraccess", "defaultconfig", "defaultsetup", "defaultinstaller", "customsetup", "custompassword", "customadmin", "readonlycustom", "readonlycustom123", "publicguest",
        "guestlogin2023", "guestuser2023", "readonlyguest", "superuseraccess", "privilegedaccess", "generalaccess", "adminuser2023", "publicpassword123", "modulepassword", "moduleaccess",
        "basicuser", "basicadmin", "basicpassword", "operatorlogin", "operatorpassword", "generalpassword", "generallogin", "generallogin123", "generalloginadmin", "tempaccess",
        "tempsetup", "temppassword", "temppassword123", "tempadmin", "backuplogin123", "backupadmin", "backupadmin123", "tempuser", "supportuser", "supportuser123",
        "supportlogin123", "servicelogin123", "oraclelogin", "postgreslogin", "mysqlaccess", "cloudlogin", "sqlaccess", "modulelogin", "unitlogin", "configlogin",
        "networkaccess123", "adminlock", "adminunlock", "admin2024", "readonlyadmin2024", "guestlogin2024", "userlogin2024", "guestadmin", "guestroot", "adminroot2024",
        "operatoraccess", "deviceadmin2023", "deviceadmin2024", "deviceuser2024", "routerlogin2024", "vpnuser2024", "vpnadmin2024", "switchlogin2024", "bridgepassword2024", "firewalladmin2024",
        "manager2024", "manageradmin2024", "guestaccess2024", "tempaccess2024", "publicaccess2024", "userpanel2024", "readonlypanel2024", "emailaccess2024", "vpnpassword2024", "networkpassword2024",
        "databasepassword2024", "superadmin2024", "routerpanel2024", "readonlyrouter", "publicpanel", "defaultuser2024", "backupaccess2024", "basicaccess", "basicaccess2024", "guestaccess2023",
        "guestloginme", "adminlogmein", "openaccessme", "guestpassme", "admin1234me", "guestloginopen", "adminloginme", "defaultuserlogin", "publicuserlogin", "networkuserlogin",
        "securelogin2024", "routeraccesslogin", "guestpanelpassword", "cloudpasswordlogin", "readonlyguestlogin", "guestlocklogin", "readonlypasswordlogin", "adminpasswordlogin", "guestlockpassword", "readonlylockpassword",
        "guest123lock", "adminlockpassword", "adminloginlock", "guestloginlock", "adminpanelpassword", "publiclockpassword", "publicaccesspassword", "publicuserpassword", "publicguestpassword", "guestguestpassword"
    };

    int passwordIndex;

    virtual void shufflePasswords(std::vector<std::string>& passwords);
};

};

#endif /* OPERATION_FACTORY_EVENT_CONCRETE_SENDHTTPREQUESTBRUTEFORCEFACTORY_H_ */
