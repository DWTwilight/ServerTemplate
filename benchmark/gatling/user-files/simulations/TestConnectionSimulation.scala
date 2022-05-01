package server_template.benchmark.simulation

import io.gatling.core.Predef._
import io.gatling.core.structure.ScenarioBuilder
import io.gatling.http.Predef._
import io.gatling.http.protocol.HttpProtocolBuilder
import io.gatling.http.request.builder.HttpRequestBuilder.toActionBuilder

/**
  * Example Gatling load test that sends one HTTP GET requests to a URL.
  * Note that the request is redirected and this causes the request count to become two.
  * Run this simulation with:
  * mvn -Dgatling.simulation.name=HttpSimulation1 gatling:test
  *
  * @author Ivan Krizsan
  */
class TestConnectionSimulation extends Simulation {
    /* Place for arbitrary Scala code that is to be executed before the simulation begins. */
    before {
        println("***** TestConnectionSimulation is about to begin! *****")
    }

    /* Place for arbitrary Scala code that is to be executed after the simulation has ended. */
    after {
        println("***** TestConnectionSimulation has ended! ******")
    }

    val host = System.getProperty("host", "localhost:8080")

    val httpProtocol = http
        .baseUrl("http://" + host)
        .acceptHeader("text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8")
        .doNotTrackHeader("1")
        .acceptLanguageHeader("en-US,en;q=0.5")
        .acceptEncodingHeader("gzip, deflate")
        .userAgentHeader("Gatling2")
        .wsBaseUrl("ws://" + host)

    val scn = scenario("WebSocket")
                .exec(ws("Connect WS").connect("/ws/echo"))
                .pause(1)
                .repeat(2, "i") {
                    exec(
                    ws("Say Hello WS")
                        .sendText("Hello")
                        .await(30)(
                        ws.checkTextMessage("checkHello").check(regex("Hello"))
                        )
                    ).pause(1)
                }
                .exec(ws("Close WS").close)
    
    setUp(
        scn.inject(rampUsers(5).during(10))
    ).protocols(httpProtocol)
}