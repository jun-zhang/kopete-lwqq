/*
    qqprotocol.cpp - Kopete QQ Protocol

    Copyright (c) 2014      by Jun Zhang             <jun.zhang@i-soft.com.cn>
    Copyright (c) 2003      by Will Stephenson		 <will@stevello.free-online.co.u>
    Kopete    (c) 2002-2003 by the Kopete developers <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This library is free software; you can redistribute it and/or         *
    * modify it under the terms of the GNU Lesser General Public            *
    * License as published by the Free Software Foundation; either          *
    * version 2 of the License, or (at your option) any later version.      *
    *                                                                       *
    *************************************************************************
*/



#include <QList>
#include <kgenericfactory.h>
#include <kdebug.h>

#include "kopeteonlinestatusmanager.h"
#include "kopeteglobal.h"
#include "kopeteproperty.h"
#include "kopeteaccountmanager.h"

#include "qqaccount.h"
#include "qqcontact.h"
#include "qqaddcontactpage.h"
#include "qqeditaccountwidget.h"

#include "qqprotocol.h"

K_PLUGIN_FACTORY( QQProtocolFactory, registerPlugin<QQProtocol>(); )
K_EXPORT_PLUGIN( QQProtocolFactory( "kopete_webqq" ) )

QQProtocol *QQProtocol::s_protocol = 0L;

QQProtocol::QQProtocol( QObject* parent, const QVariantList &/*args*/ )
	: Kopete::Protocol( QQProtocolFactory::componentData(), parent ),
	  QQLogout(  Kopete::OnlineStatus::Offline, 25, this, 0,  QStringList(QString()),
			  i18n( "Offline" ),   i18n( "Of&fline" ), Kopete::OnlineStatusManager::Offline ),
	  QQOnline(	Kopete::OnlineStatus::Online, 25, this, 0,  QStringList(QString()),
			  i18n( "Online" ),   i18n( "O&nline" ), Kopete::OnlineStatusManager::Online,Kopete::OnlineStatusManager::HasStatusMessage),
	  QQOffline(  Kopete::OnlineStatus::Offline, 25, this, 2,  QStringList(QString()),
			  i18n( "Offline" ),   i18n( "Of&fline" ), Kopete::OnlineStatusManager::Offline ),
	  QQAway(  Kopete::OnlineStatus::Away, 25, this, 1, QStringList(QLatin1String("away")),
			  i18n( "Away" ),   i18n( "&Away" ), Kopete::OnlineStatusManager::Away ),
	  QQBusy(  Kopete::OnlineStatus::Busy, 25, this, 1, QStringList(QLatin1String("Busy")),
			  i18n( "Busy" ),   i18n( "&Busy" ), Kopete::OnlineStatusManager::Busy ), 
	  QQHidden(  Kopete::OnlineStatus::Invisible, 25, this, 1, QStringList(QLatin1String("Invisible")),
			  i18n( "Invisible" ),   i18n( "&Invisible" ), Kopete::OnlineStatusManager::Invisible ), 
	  QQConnecting( Kopete::OnlineStatus::Connecting,2, this, 555, QStringList(QString::fromUtf8("qq_connecting")),    
		      i18n( "Connecting" ), i18n("Connecting"), 0, Kopete::OnlineStatusManager::HideFromMenu ),	  
	  iconCheckSum("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),	  
	  propNick("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propLongNick("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propGender("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propBirthday("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propPhone("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propProvince("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propSchool("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propEmail("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propXingzuo("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propCountry("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propCity("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propMobile("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propPage("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propJob("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty),
	  propPersonal("iconCheckSum", i18n("Buddy Icon Checksum"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty)
{
	kDebug( 14210 ) ;

	s_protocol = this;
    setCapabilities( FullRTF);
}

QQProtocol::~QQProtocol()
{
}

Kopete::Contact *QQProtocol::deserializeContact(
	Kopete::MetaContact *metaContact, const QMap<QString, QString> &serializedData,
	const QMap<QString, QString> &/* addressBookData */)
{
	QString contactId = serializedData[ "contactId" ];
	QString accountId = serializedData[ "accountId" ];
	QString displayName = serializedData[ "displayName" ];
	QString type = serializedData[ "contactType" ];

	QQContact::Type tbcType;
	if ( type == QLatin1String( "group" ) )
		tbcType = QQContact::Group;
	else if ( type == QLatin1String( "echo" ) )
		tbcType = QQContact::Echo;
	else if ( type == QLatin1String( "null" ) )
		tbcType = QQContact::Null;
	else
		tbcType = QQContact::Null;

	QList<Kopete::Account*> accounts = Kopete::AccountManager::self()->accounts( this );
	Kopete::Account* account = 0;
	foreach( Kopete::Account* acct, accounts )
	{
		if ( acct->accountId() == accountId )
			account = acct;
	}

	if ( !account )
	{
		kDebug(14210) << "Account doesn't exist, skipping";
		return 0;
	}

	QQContact * contact = new QQContact(account, contactId, displayName, metaContact);
	contact->setType( tbcType );
	return contact;
}

AddContactPage * QQProtocol::createAddContactWidget( QWidget *parent, Kopete::Account * /* account */ )
{
	kDebug( 14210 ) << "Creating Add Contact Page";
	return new QQAddContactPage( parent );
}

KopeteEditAccountWidget * QQProtocol::createEditAccountWidget( Kopete::Account *account, QWidget *parent )
{
	kDebug(14210) << "Creating Edit Account Page";
	return new QQEditAccountWidget( parent, account );
}

Kopete::Account *QQProtocol::createNewAccount( const QString &accountId )
{
	return new QQAccount( this, accountId );
}

QQProtocol *QQProtocol::protocol()
{
	return s_protocol;
}



#include "qqprotocol.moc"
