package bguspl.set.ex;

import bguspl.set.Env;

import java.util.*;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

/**
 * This class manages the dealer's threads and data
 */
public class Dealer implements Runnable {

    /**
     * The game environment object.
     */
    private final Env env;

    /**
     * Game entities.
     */
    private final Table table;
    private final Player[] players;

    /**
     * The list of card ids that are left in the dealer's deck.
     */
    private final List<Integer> deck;

    /**
     * True iff game should be terminated.
     */
    private volatile boolean terminate;

    /**
     * The time when the dealer needs to reshuffle the deck due to turn timeout.
     */
    private long reshuffleTime = Long.MAX_VALUE;

    private Thread[] playersThreads ;


    public Dealer(Env env, Table table, Player[] players) {
        this.env = env;
        this.terminate = false ;
        this.table = table;
        this.players = players;
        playersThreads = new Thread[players.length];
        deck = IntStream.range(0, env.config.deckSize).boxed().collect(Collectors.toList());
    }

    /**
     * The dealer thread starts here (main loop for the dealer thread).
     */
    @Override
    public void run() {
        env.logger.info("thread " + Thread.currentThread().getName() + " starting.");
        startPlayers();
        while (!shouldFinish()) {
            placeCardsOnTable();
            timerLoop();
//            updateTimerDisplay(false);
            removeAllCardsFromTable();
        }
        announceWinners();
        terminate();
        env.logger.info("thread " + Thread.currentThread().getName() + " terminated.");
    }
    public void startPlayers(){
        for (int i = 0 ; i < players.length ; i++){
            playersThreads[i] = new Thread(this.players[i], "Player's Id is: " + players[i].id);
            playersThreads[i].start();
        }
    }
    /**
     * The inner loop of the dealer thread that runs as long as the countdown did not time out.
     */
    private void timerLoop() {
        reshuffleTime = System.currentTimeMillis() + env.config.turnTimeoutMillis;
        while (!terminate && System.currentTimeMillis() < reshuffleTime) {
            //sleepUntilWokenOrTimeout();
            updateTimerDisplay(false);
            removeCardsFromTable();
            placeCardsOnTable();
        }
    }



    /**
     * Called when the game should be terminated.
     */
    public void terminate() {
        // TODO implement
        for(int i = players.length-1 ; i>=0 ; i--){
            players[i].terminate();
        }
        this.terminate = true ;
    }

    /**
     * Check if the game should be terminated or the game end conditions are met.
     *
     * @return true iff the game should be finished.
     */
    private boolean shouldFinish() {
        return terminate || env.util.findSets(deck, 1).size() == 0;
    }

    /**
     * Checks cards should be removed from the table and removes them.
     */
    private void removeCardsFromTable() {
        // TODO implement
        if(!table.reached3Players.isEmpty()){
            int playerId = table.reached3Players.poll();
            LinkedList<Integer> l = table.playersTokens[playerId];//change this line
            if(l.size()!=3){
                return;
            }
            int [] arr = listToArray(l) ;
            if(IsSet(l)){
                players[playerId].dealerResponded = true ;
                players[playerId].isPoint = true;
                players[playerId].isPenalty = false;
                table.playersTokens[playerId].clear();
                table.reached3Players.remove(playerId);

                for(int card : arr ){
                    for(int i = 0 ; i < players.length ;i++){
                        if(table.playersTokens[i].contains(card)){
                            table.removeToken(i ,card);
                        }
                    }
                    table.removeCard(card);
                }
                reshuffleTime = System.currentTimeMillis() + env.config.turnTimeoutMillis;
            }else{
                players[playerId].dealerResponded = true ;
                players[playerId].isPenalty = true;
                players[playerId].isPoint = false;

            }
        }
    }
    public int[] listToArray(LinkedList<Integer> list){// change this accordingly
        int[] output = new int[3] ;
        Iterator<Integer> itr = list.iterator();
        int idx = 0 ;
        while(itr.hasNext() && idx <= output.length){
            output[idx] = itr.next();
            idx ++ ;
        }
        return  output ;
    }
    private  boolean IsSet(LinkedList<Integer> list) {//change this
        if(list ==null||list.size()!=3){
            return false;
        }else{
            int[] a = listToArray(list);
            a[0] = table.slotToCard[a[0]];
            a[1] = table.slotToCard[a[1]];
            a[2] = table.slotToCard[a[2]];
            return env.util.testSet(a);
        }

    }

    /**
     * Check if any cards can be removed from the deck and placed on the table.
     */
    private void placeCardsOnTable() {
        // TODO implement
        table.placingCards=true;
        LinkedList<Integer> cards = new LinkedList<>();
        for (int i = 0; i < table.slotToCard.length; i++) {
            if (table.slotToCard[i] == null) {
                cards.add(i);
            }
        }


        Collections.shuffle(cards);
        Collections.shuffle(deck);
        int cardsSize = cards.size();
        for (int i = 0; i < cardsSize; i++) {
            if (!deck.isEmpty())
                table.placeCard(deck.remove(0), cards.get(i));
            else {
                terminate();
            }
        }
        table.placingCards=false;
    }

    /**
     * Sleep for a fixed amount of time or until the thread is awakened for some purpose.
     */
    private void sleepUntilWokenOrTimeout() {
        // TODO implement
    }

    /**
     * Reset and/or update the countdown and the countdown display.
     */
    private void updateTimerDisplay(boolean reset) {
        // TODO implement
        long remainingTime = reshuffleTime - System.currentTimeMillis();
        if (remainingTime < 0) {
            remainingTime = 0;
        }
        if (reset == true) {
            env.ui.setCountdown(env.config.turnTimeoutMillis, false);
            reshuffleTime = System.currentTimeMillis() + env.config.turnTimeoutMillis;
        } else if (remainingTime < env.config.turnTimeoutWarningMillis) {
            env.ui.setCountdown(remainingTime, true);
        } else {
            env.ui.setCountdown(remainingTime, false);
        }
    }

    /**
     * Returns all the cards from the table to the deck.
     */
    private void removeAllCardsFromTable() {
        // TODO implement
        table.placingCards=true;
        for(int i=0;i<env.config.tableSize;i++){
            if(table.slotToCard[i] != null) {
                deck.add(table.slotToCard[i]);
                table.removeCard(i);
            }
        }
        removeAllSelectedTokens() ;
        table.placingCards=false;
    }
    private void removeAllSelectedTokens(){
        for(int playerId = 0 ; playerId < players.length ; playerId++) {
            table.playersTokens[playerId].clear();
        }
    }

    /**
     * Check who is/are the winner/s and displays them.
     */
    private void announceWinners() {
        // TODO implement
        int maxScore = 0 ;
        for(int i = 0 ; i < env.config.players ; i++){
            if(players[i].score() > players[maxScore].score() )
                maxScore = i ;
        }
        int countWinners = 0 ;
        for(int i = 0 ; i < env.config.players ; i++){
            if(players[i].score() == players[maxScore].score())
                countWinners ++ ;
        }
        int[] listOfWinners = new int[countWinners] ;
        int idx = 0 ;
        for(int i = 0 ; i < players.length && idx<countWinners ; i++){
            if(players[i].score() == players[maxScore].score()){
                listOfWinners[idx] = players[i].id ;
                idx++ ;
            }
        }
        env.ui.announceWinner(listOfWinners);
    }
}