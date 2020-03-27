/*
 * Copyright (c) 2015 Cryptonomex, Inc., and contributors.
 *
 * The MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <graphene/chain/hardfork.hpp>

#include <graphene/chain/fba_accumulator_id.hpp>

#include <graphene/chain/fba_object.hpp>
#include <graphene/chain/market_object.hpp>
#include <graphene/chain/proposal_object.hpp>
#include <graphene/chain/vesting_balance_object.hpp>
#include <graphene/chain/exceptions.hpp>

#include <fc/uint128.hpp>

#include <boost/test/unit_test.hpp>

#include "../common/database_fixture.hpp"

using namespace graphene::chain;
using namespace graphene::chain::test;

BOOST_FIXTURE_TEST_SUITE( fee_tests, database_fixture )

BOOST_AUTO_TEST_CASE( bsip86_hardfork_test )
{
   {
      // committee cannot set the fee before the hardfork
      proposal_create_operation cop = proposal_create_operation::committee_proposal(
            db.get_global_properties().parameters, db.head_block_time());
      cop.fee_paying_account = GRAPHENE_TEMP_ACCOUNT;
      cop.expiration_time = db.head_block_time() + *cop.review_period_seconds + 10;
      committee_member_update_global_parameters_operation cmuop;
      cmuop.new_parameters.extensions.value.market_fee_network_percent = 1;
      cop.proposed_ops.emplace_back(cmuop);
      trx.operations.push_back(cop);
      BOOST_TEST_MESSAGE("Attempting to push the proposal");
      GRAPHENE_CHECK_THROW( PUSH_TX( db, trx ), fc::exception );
      BOOST_TEST_MESSAGE("Verifying that proposal did not succeed.");
      BOOST_CHECK(!db.get_global_properties().parameters.extensions.value.market_fee_network_percent.valid());
      trx.clear();
   }

   {
      // committee can set the fee after the hardfork
      generate_blocks( HARDFORK_CORE_BSIP86_TIME + 10 );
      set_expiration(db, trx);
      BOOST_CHECK(!db.get_global_properties().parameters.extensions.value.market_fee_network_percent.valid());
      proposal_create_operation cop = proposal_create_operation::committee_proposal(
            db.get_global_properties().parameters, db.head_block_time());
      cop.fee_paying_account = GRAPHENE_TEMP_ACCOUNT;
      cop.expiration_time = db.head_block_time() + *cop.review_period_seconds + 10;
      committee_member_update_global_parameters_operation cmuop;
      cmuop.new_parameters.extensions.value.market_fee_network_percent = 1;
      cop.proposed_ops.emplace_back(cmuop);
      trx.operations.push_back(cop);
      BOOST_TEST_MESSAGE("Pushing the good proposal");
      const proposal_id_type prop_id = PUSH_TX( db, trx ).operation_results[0].get<object_id_type>();
      trx.clear();
      // update with signatures
      proposal_update_operation uop;
      uop.proposal = prop_id;
      uop.fee_paying_account = GRAPHENE_TEMP_ACCOUNT;
      uop.active_approvals_to_add = {get_account("init0").get_id(), get_account("init1").get_id(),
                                    get_account("init2").get_id(), get_account("init3").get_id(),
                                    get_account("init4").get_id(), get_account("init5").get_id(),
                                    get_account("init6").get_id(), get_account("init7").get_id()};
      trx.operations.push_back(uop);
      sign( trx, init_account_priv_key );
      BOOST_TEST_MESSAGE("Sending good proposal.");
      db.push_transaction(trx);
      BOOST_TEST_MESSAGE("Moving beyond good proposal expiration time");
      generate_blocks( db.head_block_time() + *cop.review_period_seconds + 15);
      BOOST_TEST_MESSAGE("Moving beyond the next maintenance interval");
      generate_blocks( db.get_dynamic_global_properties().next_maintenance_time + 10);
      BOOST_TEST_MESSAGE("Verifying that proposal did succeed.");
      BOOST_CHECK(db.get_global_properties().parameters.extensions.value.market_fee_network_percent.valid());
      trx.clear();
   }
}

BOOST_AUTO_TEST_SUITE_END()