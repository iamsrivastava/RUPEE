// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "main.h"
#include "uint256.h"

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    // How many times we expect transactions after the last checkpoint to
    // be slower. This number is a compromise, as it can't be accurate for
    // every system. When reindexing from a fast disk with a slow CPU, it
    // can be up to 20, while when downloading from a slow network with a
    // fast multicore CPU, it won't be much higher than 1.
    static const double fSigcheckVerificationFactor = 5.0;

    struct CCheckpointData {
        const MapCheckpoints *mapCheckpoints;
        int64 nTimeLastCheckpoint;
        int64 nTransactionsLastCheckpoint;
        double fTransactionsPerDay;
    };

    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        (  0, uint256("0xfb6a19a31293de12035e5cdaadce2a8552d3feeec6fa5f910101d9d8c579b566"))
        (  5, uint256("0x990a966a5bdc7554f4d2416991052e6bc24a5d0b97ed1fe663b502673f609667"))
        ( 56, uint256("0x7a0eb0bdaf399e37a865897c6bb385a444b90cec45a684671b1efd6f2de3bb48"))
        (113, uint256("0x611447a153975ec76c3cd39caacce099f985fd361ff32116161b3140c87df8aa"))
        (581, uint256("0x03409ddba296135417bb50e47d00af87c928c82672cc60a2d91eb6b7b43b1346"))
        (1584, uint256("0x1539677547b7e39de13e8ff69add07a7c5b69f9f2a07674078ca6a12581584c0"))
        (5048, uint256("0x44756946793ef59f493a5dfe6f6dc3ea6d01821959d2fef1605f6ee907788bc6"))
        (16842, uint256("0x81b49335e8627353b51820d5d2ae00754f4fd10a5eee154a867f1699cbccc82c"))
		(20625, uint256("0xb84ed271f5541a7409604ea23771f452968229759e207ffd8bf1e79259d7b06e"))
		(27548, uint256("0x42b51999e185e5c0c8d73f6abe809158c3a26d8120f72c2bbcabe28239a9c078"))
		(35249, uint256("0x615d8a1ac0ac0090e6b3bea6decd2e28d646903a64dd818a0a03ca091a96a0c6"))
		(47101, uint256("0x53e80e86aa8315dcd05f8ba7595fb810b4b5b795284844264ae0c1330a332da3"))
        ;
    static const CCheckpointData data = {
        &mapCheckpoints,
    };

    static MapCheckpoints mapCheckpointsTestnet =
        boost::assign::map_list_of
		(0, uint256("0x83b9153a393cb8b9c4857932f37b2c3fb8ce65077029b8f7e3b2db11a7fae0d1"))
		(1, uint256("0x49819431645311975fbebb7428dc4741064fe1045f20fb09aafd8365b75134ed"))
		(59, uint256("0x81833f79c205122ed6bd810f4a1bf3ae3782a3e8998511913b348c012609d8a3"))
		(127, uint256("0x23535df5a0c3539ad61a64e7f556b54a2aae83dc85dc347ef07ae953a344966a"))
		(982, uint256("0xc6c38c8007b5e2f804b79c646883b960b7b6a1d1ab587cd15b05ae1810f3e030"))
		(1625, uint256("0x8be9c4a925a915355c668b8cd8da4d0a1833f7b7447f2d82bfd8a586bdc3c8d0"))
		(2187, uint256("0x79ed73d5b20912e92851e7b00f9f8f68f0ac49bab9147d45712c24dbb72e6df3"))
		(2845, uint256("0xd79ba9298c6ead84df8827703b5231250f35a956329be7edacae9253b5f8cbee"))
		(3549, uint256("0xd3947d507db61372f27838a8693b90a9a43d8a629010fac0c1514ae07672ba10"))
		(5012, uint256("0xbff1416e7cc022fb07fdb1a570e812e05320a637ca33444c92dd818a7ad0d3f0"))
		(6010, uint256("0x492c3eeca4e32ca162b656f5f7b2ac81f3b995523216abc9fb41d1e3e31a04c7"))
        ;
    static const CCheckpointData dataTestnet = {
        &mapCheckpointsTestnet,
    };

    const CCheckpointData &Checkpoints() {
        if (fTestNet)
            return dataTestnet;
        else
            return data;
    }

    bool CheckBlock(int nHeight, const uint256& hash)
    {
        if (!GetBoolArg("-checkpoints", true))
            return true;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    // Guess how far we are in the verification process at the given block index
    double GuessVerificationProgress(CBlockIndex *pindex) {
        if (pindex==NULL)
            return 0.0;

        int64 nNow = time(NULL);

        double fWorkBefore = 0.0; // Amount of work done before pindex
        double fWorkAfter = 0.0;  // Amount of work left after pindex (estimated)
        // Work is defined as: 1.0 per transaction before the last checkoint, and
        // fSigcheckVerificationFactor per transaction after.

        const CCheckpointData &data = Checkpoints();

        if (pindex->nChainTx <= data.nTransactionsLastCheckpoint) {
            double nCheapBefore = pindex->nChainTx;
            double nCheapAfter = data.nTransactionsLastCheckpoint - pindex->nChainTx;
            double nExpensiveAfter = (nNow - data.nTimeLastCheckpoint)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore;
            fWorkAfter = nCheapAfter + nExpensiveAfter*fSigcheckVerificationFactor;
        } else {
            double nCheapBefore = data.nTransactionsLastCheckpoint;
            double nExpensiveBefore = pindex->nChainTx - data.nTransactionsLastCheckpoint;
            double nExpensiveAfter = (nNow - pindex->nTime)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore + nExpensiveBefore*fSigcheckVerificationFactor;
            fWorkAfter = nExpensiveAfter*fSigcheckVerificationFactor;
        }

        return fWorkBefore / (fWorkBefore + fWorkAfter);
    }

    int GetTotalBlocksEstimate()
    {
        if (!GetBoolArg("-checkpoints", true))
            return 0;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        if (!GetBoolArg("-checkpoints", true))
            return NULL;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }
}
