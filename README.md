# binglib

*Bitcoin Wallet Helper Library*

**License Overview**

All files in this repository fall under the license specified in [COPYING](COPYING). The project is licensed as [AGPL with a lesser clause](https://wiki.unsystem.net/en/index.php/Libbitcoin/License). It may be used within a proprietary project, but the core library and any changes to it must be published online. Source code for this library must always remain free for everybody to access.

**About Binglib**

The library contains a variety of utilities to be used by a Bitcoin wallet,
the library uses Libbitcoin and Electrum Server API.
The functionality includes:
- ElectrumInterface for a subset of Electrum Server API
- History Inspector which calculates wallet balance and provides itemised wallet history
- Funds Finder chooses a set of UTXOs which satisfy funding requirement at an individual address scope
- Purse Accessor chooses a set of UTXOs which satisfy funding requirement at wallet (set of addresses) scope
- Libbitcoin client for a small subset of Libbitcoin API
- Redeem Script for CLTV (checklocktimeverify) operation
- OnlineLockTxCreator creates a transaction (P2SH) which locks funds from a given address
- OnlineP2pkhTxCreator creates a transaction (P2PKH) which transfers funds between addresses
- OfflineUnlockTxCreator creates a transaction (P2PKH) which unlocks and transfers funds
- BingWallet derives addresses from a given Electrum format seed phrase
- RohghuaClient implements ElectrumInterface providing access to Electrum Server API
- WalletState holds wallet state such as addresses and transactions history
- AddressConverter converts base58 address format to Script Public Key Hash format

The above functionality provides core functions for the Bitcoin wallet.
Focus of the functionality is on CLTV operation, to allow the wallet to be able to
lock and unlock funds at a given Bitcoin address.

**Limitations**

At the moment, the library does not support Segwit, it only works with legacy addresses.
In the next phase, the library will be extended to support Segwit and Taproot addresses.



