#ifndef CF_GUI_Client_ClientCore_hpp
#define CF_GUI_Client_ClientCore_hpp

////////////////////////////////////////////////////////////////////////////////

#include <QObject>

#include "Common/XmlHelpers.hpp"

#include "GUI/Client/TSshInformation.hpp"

#include "GUI/Client/CNode.hpp"

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

////////////////////////////////////////////////////////////////////////////////

  /// @brief Core of the client application.

  /// This class is a singleton and a reference to the unique instance can be
  /// obtained by calling @c #instance() function. This class is non-copyable. @n
  /// This class is an interface between the client network layer and the rest
  /// of the application. Because it is a singleton, the network layer is
  /// accessible from everywhere.

  class NCore :
      public QObject,
      public CNode
  {
    Q_OBJECT

  public:

    typedef boost::shared_ptr<NCore> Ptr;

    /// @brief Constructor
    NCore();

    /// @brief Destructor
    ~NCore();

    /// @brief Sends a signal to the network layer

    /// @param signal The signal to send. Build the signal using @c #XmlOps and
    /// @c #XmlParams classes.
    void sendSignal(CF::Common::XmlDoc & signal);

    /// @brief Attempts to connect to a server.

    /// @param sshInfo Connection information
    void connectToServer(const TSshInformation & sshInfo);

    void disconnectFromServer(bool shutdown);

    /// @brief Gives the icon associated to this node
    /// @return Returns the icon associated to this node
    /// @note This method should be reimplemented by all subclasses.
    virtual QIcon getIcon() const;

    /// @brief Gives the text to put on a tool tip
    /// @return The name of the class.
    virtual QString getToolTip() const;

  public slots:

    void updateTree();

  private slots:

    /// @brief Slot called when the client is connected to the server.
    void connected();

  signals:

    void connectedToServer();

    void disconnectedFromServer();

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

    /// regists all the signals declared in this class
    static void regist_signals ( Component* self ) {}

    /// @name Signals
    //@{

    void shutdown(CF::Common::XmlNode & node);

    void client_registration(CF::Common::XmlNode & node);

    void frame_rejected(CF::Common::XmlNode & node);

    //@} END Signals

  }; // class ClientCore

////////////////////////////////////////////////////////////////////////////////

} // namespace Client
} // namespace GUI
} // namespace CF

////////////////////////////////////////////////////////////////////////////////

#endif // CF_GUI_Client_ClientCore_hpp
