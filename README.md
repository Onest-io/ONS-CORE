[![Build Status](https://travis-ci.org/Onest-io/ONS-CORE.svg?branch=master)](https://travis-ci.org/Onest-io/ONS-CORE)

Onest Blockchain Core Node
==============

* [Getting Started](#getting-started)
* [Support](#support)
* [Using the API](#using-the-api)
* [Accessing restricted API's](#accessing-restricted-apis)
* [FAQ](#faq)
* [License](#license)

Getting started with installing onest blockchain core node:
---------------
Build instructions and additional documentation are available in the
[wiki](https://github.com/Onest-io/ONS-CORE/wiki).

| Default | High-end | Minimal  | Elastic-Search 
| --- | --- | --- | ---
| 200G HDD, 16G RAM | 1TB SSD, 64G RAM * | 100G HDD, 4G RAM | 500G SSD, 32G RAM

\* For this setup, allocate at least 500GB of SSD as swap.

**NOTE:** Versions of [Boost](http://www.boost.org/) 1.58 through 1.69 are supported. Newer versions may work, but
have not been tested. If your system came pre-installed with a version of Boost that you do not wish to use, you may
manually build your preferred version and use it with Onest by specifying it on the CMake command line.

Example: ``cmake -DBOOST_ROOT=/path/to/boost .``

**NOTE:** Onest requires a 64-bit operating system to build, and will not build on a 32-bit OS.

**NOTE:** Onest now supports Ubuntu 18.04 LTS

**NOTE:** Onest recommend building on Ubuntu 16.04 LTS (64-bit) 

**NOTE:** Onest now supports OpenSSL 1.1.0


**1) Build Dependencies:**

    sudo apt-get update
    sudo apt-get install autoconf cmake make automake libtool git libboost-all-dev libssl-dev g++ libcurl4-openssl-dev

**2) Build From Source Code:**

    git clone https://github.com/Onest-io/ONS-CORE.git
    cd ONS-CORE
    git checkout master # may substitute "master" with current release tag
    git submodule update --init --recursive --remote
    cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .
    make

**3) After Building:** 

Note: you have to run`witness_node` to start the node, it can be launched with the below command while, making sure to `cd ~/ONS-CORE` before launching this below command, if you want to keep the node running in background you might need to run it after entering `screen` session then run it, to exit the screen session and keep the node running, hold on the `ctrl` key and then press once `A` then `D` once again while holding `ctrl` key on your keyboard, to get back to session use the command `screen -r`.

The command to run the node is:

    ./programs/witness_node/witness_node

Note: The node will automatically create a data directory including a config file. It may take several hours to fully synchronize
the blockchain. After syncing, you can exit the node using Ctrl+C and setup the command-line wallet by editing
`witness_node_data_dir/config.ini` as follows:

    rpc-endpoint = 127.0.0.1:8090

**IMPORTANT:** By default the witness node will start in reduced memory mode by using some of the commands detailed in [Memory reduction for nodes](https://github.com/ONS-CORE/wiki/Memory-reduction-for-nodes).
In order to run a full node with all the account history you need to remove `partial-operations` and `max-ops-per-account` from your config file. Please note that currently(2018-10-17) a full node will need more than 160GB of RAM to operate and required memory is growing fast. Consider the following table as minimal requirements before running a node:

After starting the witness node again, in a separate terminal you can run:

    ./programs/cli_wallet/cli_wallet

Set your inital password:

    >>> set_password <PASSWORD>
    >>> unlock <PASSWORD>

**IMPORTANT:** The cli_wallet or API interfaces to the witness node wouldn't be fully functional unless the witness node is fully synchronized with the blockchain. The cli_wallet command `info` will show result `head_block_age` which will tell you how far you are from the live current block of the blockchain.


To check your current block:

    >>> info

To import your initial balance:

    >>> import_balance <ACCOUNT NAME> [<WIF_KEY>] true

If you send private keys over this connection, `rpc-endpoint` should be bound to localhost for security.

Use `help` to see all available wallet commands. Source definition and listing of all commands is available
[here](https://github.com/Onest-io/ONS-CORE/blob/master/libraries/wallet/include/graphene/wallet/wallet.hpp).

Upgrading an existing onest blockchain core node:
---------------
In case you don't want to resync the entire blockchain again by performing the new installation procedure mentioned above, you can upgrade the node using the following commands while, making sure to `cd ~/ONS-CORE` directory before launching these commands, make sure to kill the existing node `witness_node` process before you start the upgrade and remember to start `witness_node` once you are done:

    git remote set-url origin https://github.com/Onest-io/ONS-CORE.git
    git checkout master
    git remote set-head origin --auto
    git pull
    git submodule update --init --recursive --remote # this command may fail
    git submodule sync --recursive
    git submodule update --init --recursive --remote
    

Support
-------

Onest Core bugs can be reported directly to the [issue tracker](https://github.com/Onest-io/ONS-CORE/issues).

Onest UI bugs should be reported to the [UI issue tracker](https://github.com/Onest-io/ONS-CORE/issues)


Using the API
-------------

We provide several different API's.  Each API has its own ID.
When running `witness_node`, initially two API's are available:
API 0 provides read-only access to the database, while API 1 is
used to login and gain access to additional, restricted API's.

Here is an example using `wscat` package from `npm` for websockets:

    $ npm install -g wscat
    $ wscat -c ws://127.0.0.1:8090
    > {"id":1, "method":"call", "params":[0,"get_accounts",[["1.2.0"]]]}
    < {"id":1,"result":[{"id":"1.2.0","annotations":[],"membership_expiration_date":"1969-12-31T23:59:59","registrar":"1.2.0","referrer":"1.2.0","lifetime_referrer":"1.2.0","network_fee_percentage":2000,"lifetime_referrer_fee_percentage":8000,"referrer_rewards_percentage":0,"name":"committee-account","owner":{"weight_threshold":1,"account_auths":[],"key_auths":[],"address_auths":[]},"active":{"weight_threshold":6,"account_auths":[["1.2.5",1],["1.2.6",1],["1.2.7",1],["1.2.8",1],["1.2.9",1],["1.2.10",1],["1.2.11",1],["1.2.12",1],["1.2.13",1],["1.2.14",1]],"key_auths":[],"address_auths":[]},"options":{"memo_key":"GPH1111111111111111111111111111111114T1Anm","voting_account":"1.2.0","num_witness":0,"num_committee":0,"votes":[],"extensions":[]},"statistics":"2.7.0","whitelisting_accounts":[],"blacklisting_accounts":[]}]}

We can do the same thing using an HTTP client such as `curl` for API's which do not require login or other session state:

    $ curl --data '{"jsonrpc": "2.0", "method": "call", "params": [0, "get_accounts", [["1.2.0"]]], "id": 1}' http://127.0.0.1:8090/rpc
    {"id":1,"result":[{"id":"1.2.0","annotations":[],"membership_expiration_date":"1969-12-31T23:59:59","registrar":"1.2.0","referrer":"1.2.0","lifetime_referrer":"1.2.0","network_fee_percentage":2000,"lifetime_referrer_fee_percentage":8000,"referrer_rewards_percentage":0,"name":"committee-account","owner":{"weight_threshold":1,"account_auths":[],"key_auths":[],"address_auths":[]},"active":{"weight_threshold":6,"account_auths":[["1.2.5",1],["1.2.6",1],["1.2.7",1],["1.2.8",1],["1.2.9",1],["1.2.10",1],["1.2.11",1],["1.2.12",1],["1.2.13",1],["1.2.14",1]],"key_auths":[],"address_auths":[]},"options":{"memo_key":"GPH1111111111111111111111111111111114T1Anm","voting_account":"1.2.0","num_witness":0,"num_committee":0,"votes":[],"extensions":[]},"statistics":"2.7.0","whitelisting_accounts":[],"blacklisting_accounts":[]}]}

API 0 is accessible using regular JSON-RPC:

    $ curl --data '{"jsonrpc": "2.0", "method": "get_accounts", "params": [["1.2.0"]], "id": 1}' http://127.0.0.1:8090/rpc

Accessing restricted API's
--------------------------

You can restrict API's to particular users by specifying an `api-access` file in `config.ini` or by using the `--api-access /full/path/to/api-access.json` startup node command.  Here is an example `api-access` file which allows
user `bytemaster` with password `supersecret` to access four different API's, while allowing any other user to access the three public API's
necessary to use the wallet:

    {
       "permission_map" :
       [
          [
             "iobanker",
             {
                "password_hash_b64" : "9e9GF7ooXVb9k4BoSfNIPTelXeGOZ5DrgOYMj94elaY=",
                "password_salt_b64" : "INDdM6iCi/8=",
                "allowed_apis" : ["database_api", "network_broadcast_api", "history_api", "network_node_api"]
             }
          ],
          [
             "*",
             {
                "password_hash_b64" : "*",
                "password_salt_b64" : "*",
                "allowed_apis" : ["database_api", "network_broadcast_api", "history_api"]
             }
          ]
       ]
    }

Passwords are stored in `base64` as salted `sha256` hashes.  A simple Python script, `saltpass.py` is avaliable to obtain hash and salt values from a password.
A single asterisk `"*"` may be specified as username or password hash to accept any value.

With the above configuration, here is an example of how to call `add_node` from the `network_node` API:

    {"id":1, "method":"call", "params":[1,"login",["iobanker", "supersecret"]]}
    {"id":2, "method":"call", "params":[1,"network_node",[]]}
    {"id":3, "method":"call", "params":[2,"add_node",["127.0.0.1:9090"]]}

Note, the call to `network_node` is necessary to obtain the correct API identifier for the network API.  It is not guaranteed that the network API identifier will always be `2`.

Since the `network_node` API requires login, it is only accessible over the websocket RPC.  Our `doxygen` documentation contains the most up-to-date information
about API's for the [witness node] and the
[wallet].

FAQ
---

- Is there a way to generate help with parameter names and method descriptions?

    Yes. Documentation of the code base, including APIs, can be generated using Doxygen. Simply run `doxygen` in this directory.

    If both Doxygen and perl are available in your build environment, the CLI wallet's `help` and `gethelp`
    commands will display help generated from the doxygen documentation.

    If your CLI wallet's `help` command displays descriptions without parameter names like
        `signed_transaction transfer(string, string, string, string, string, bool)`
    it means CMake was unable to find Doxygen or perl during configuration.  If found, the
    output should look like this:
        `signed_transaction transfer(string from, string to, string amount, string asset_symbol, string memo, bool broadcast)`

- Is there a way to allow external program to drive `cli_wallet` via websocket, JSONRPC, or HTTP?

    Yes. External programs may connect to the CLI wallet and make its calls over a websockets API. To do this, run the wallet in
    server mode, i.e. `cli_wallet -s "127.0.0.1:9999"` and then have the external program connect to it over the specified port
    (in this example, port 9999).

- Is there a way to access methods which require login over HTTP?

    No.  Login is inherently a stateful process (logging in changes what the server will do for certain requests, that's kind
    of the point of having it).  If you need to track state across HTTP RPC calls, you must maintain a session across multiple
    connections.  This is a famous source of security vulnerabilities for HTTP applications.  Additionally, HTTP is not really
    designed for "server push" notifications, and we would have to figure out a way to queue notifications for a polling client.

    Websockets solves all these problems.  If you need to access Graphene's stateful methods, you need to use Websockets.

- What is the meaning of `a.b.c` numbers?

    The first number specifies the *space*.  Space 1 is for protocol objects, 2 is for implementation objects.
    Protocol space objects can appear on the wire, for example in the binary form of transactions.
    Implementation space objects cannot appear on the wire and solely exist for implementation
    purposes, such as optimization or internal bookkeeping.

    The second number specifies the *type*.  The type of the object determines what fields it has.  For a
    complete list of type ID's, see `enum object_type` and `enum impl_object_type` in
    [types.hpp](https://github.com/Onest-io/ONS-CORE/blob/onest/libraries/chain/include/graphene/chain/protocol/types.hpp).

    The third number specifies the *instance*.  The instance of the object is different for each individual
    object.

- The answer to the previous question was really confusing.  Can you make it clearer?

    All account ID's are of the form `1.2.x`.  If you were the 9735th account to be registered,
    your account's ID will be `1.2.9735`.  Account `0` is special (it's the "committee account,"
    which is controlled by the committee members and has a few abilities and restrictions other accounts
    do not).

    All asset ID's are of the form `1.3.x`.  If you were the 29th asset to be registered,
    your asset's ID will be `1.3.29`.  Asset `0` is special (it's ONS, which is considered the "core asset").

    The first and second number together identify the kind of thing you're talking about (`1.2` for accounts,
    `1.3` for assets).  The third number identifies the particular thing.

- How do I get the `network_add_nodes` command to work?  Why is it so complicated?

    You need to follow the instructions in the "Accessing restricted API's" section to
    allow a username/password access to the `network_node` API.  Then you need
    to pass the username/password to the `cli_wallet` on the command line or in a config file.

    It's set up this way so that the default configuration is secure even if the RPC port is
    publicly accessible.  It's fine if your `witness_node` allows the general public to query
    the database or broadcast transactions (in fact, this is how the hosted web UI works).  It's
    less fine if your `witness_node` allows the general public to control which p2p nodes it's
    connecting to.  Therefore the API to add p2p connections needs to be set up with proper access
    controls.
 
License
-------
Onest Core Code is under the MIT license. See [LICENSE](https://github.com/Onest-io/ONS-CORE/blob/master/LICENSE.txt)
for more information.
