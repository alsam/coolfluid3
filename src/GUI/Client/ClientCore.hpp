#ifndef CF_GUI_Client_ClientCore_hpp
#define CF_GUI_Client_ClientCore_hpp

////////////////////////////////////////////////////////////////////////////////

#include <QMap>
#include <QObject>

#include "Common/XmlHelpers.hpp"
#include "Common/SignalHandler.hpp"

#include "GUI/Client/TSshInformation.hpp"

class QModelIndex;
class QProcess;
class QString;
class QTimer;

////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace GUI {

namespace Network {
  class ComponenentType;
  struct HostInfos;
  class SignalInfo;
}

namespace Client {

////////////////////////////////////////////////////////////////////////////////

  class ClientNetworkComm;
  class StatusModel;
  struct TSshInformation;

////////////////////////////////////////////////////////////////////////////////

  /// @brief Core of the client application.

  /// This class is a singleton and a reference to the unique instance can be
  /// obtained by calling @c #instance() function. This class is non-copyable. @n
  /// This class is an interface between the client network layer and the rest
  /// of the application. Because it is a singleton, the network layer is
  /// accessible from everywhere.

  class ClientCore :
      public QObject,
      public boost::noncopyable
  {
    Q_OBJECT

  public:

    /// @brief Builds and gives the unique instance.

    /// @return Returns a reference to the unique object.
    static ClientCore & instance();

    /// @brief Sends a signal to the network layer

    /// @param signal The signal to send. Build the signal using @c #XmlOps and
    /// @c #XmlParams classes.
    void sendSignal(CF::Common::XmlDoc & signal);

    /// @brief Attempts to connect to a server.

    /// @param sshInfo Connection information
    void connectToServer(const TSshInformation & sshInfo);

    void disconnectFromServer(bool shutdown);

  private slots:

    /// @brief Slot called when the client is connected to the server.
    void connected();

    /// @brief Attempts to connect to the server.

    /// During the waiting for the server to launch through an SSH
    /// connection, this slot is called at every timeout of
    /// @c #timer and tries to connect to the server.
    void tryToConnect();

    /// @brief Slot called whenever an error occurs during the server launching.

    /// Any error or warning is considered as critical and stops the launching
    /// process immediately.
    void sshError();

  signals:

    void connectedToServer();

    void disconnectedFromServer();

  private: // methods

    /// @brief Constructor
    ClientCore();

    /// @brief Destructor
    ~ClientCore();

  private: // data

    /// @brief The network layer
    ClientNetworkComm * m_networkComm;

    /// @brief Timer used on server launchin process.

    /// On timeout, the client attemps to connect to the server.
    QTimer * m_timer;

    /// @brief The launching process.
    QProcess * m_process;

    /// @brief The current connection information.
    TSshInformation m_commSshInfo;

  }; // class ClientCore

////////////////////////////////////////////////////////////////////////////////

} // namespace Client
} // namespace GUI
} // namespace CF

////////////////////////////////////////////////////////////////////////////////

#endif // CF_GUI_Client_ClientCore_hpp
