package server_template

import io.gatling.core.Predef._
import io.gatling.core.structure.ScenarioBuilder
import io.gatling.http.Predef._
import io.gatling.http.protocol.HttpProtocolBuilder

/**
  * Example Gatling load test that sends one HTTP GET requests to a URL.
  * Note that the request is redirected and this causes the request count to become two.
  * Run this simulation with:
  * mvn -Dgatling.simulation.name=HttpSimulation1 gatling:test
  *
  * @author Ivan Krizsan
  */
class TestMessageSimulation extends Simulation {
    /* Place for arbitrary Scala code that is to be executed before the simulation begins. */
    before {
        println("***** TestMessageSimulation is about to begin! *****")
    }

    /* Place for arbitrary Scala code that is to be executed after the simulation has ended. */
    after {
        println("***** TestMessageSimulation has ended! ******")
    }

    val host = System.getProperty("host", "0.0.0.0:8080")
    val path = System.getProperty("path", "/echo")
    val users = Integer.getInteger("users", 1).intValue()

    val httpProtocol = http
        .baseUrl("http://" + host)
        .acceptHeader("text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8")
        .doNotTrackHeader("1")
        .acceptLanguageHeader("en-US,en;q=0.5")
        .acceptEncodingHeader("gzip, deflate")
        .userAgentHeader("Gatling2")
        .wsBaseUrl("ws://" + host)

    val scn = scenario("WebSocket")
                .exec(ws("Connect WS").connect(path))
                .pause(5)
                .repeat(users, "i") {
                    exec(
                    ws("Say Hello WS")
                        .sendText("Hello")
                        .await(30)(
                        ws.checkTextMessage("checkHello").check(regex("Hello"))
                        )
                    )
                }
                .exec(ws("Close WS").close)
    
    setUp(
        scn.inject(atOnceUsers(users))
    ).protocols(httpProtocol)
}