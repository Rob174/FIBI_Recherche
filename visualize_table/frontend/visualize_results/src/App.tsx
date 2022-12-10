import "./App.css";
import { Route, Link, Routes } from "react-router-dom";
import { Averages } from "./components/Averages";
import { DiffImprShapes } from "./components/DiffImprShapes";
import { InitDistr } from "./components/InitDistr";
import { Stats } from "./components/Stats";
import { useEffect } from "react";
export default function App() {
  useEffect(() => {
    axios.get(`https://localhost:5000`)
      .then(res => {
        const persons = res.data;
        this.setState({ persons });
      })
    console.log()
  })
  return (
    <div className="App">
      <ul>
        <li>
          <Link to="/averages">Averages</Link>
        </li>
        <li>
          <Link to="/intial_distr">Initialization distribution</Link>
        </li>
        <li>
          <Link to="/diff_impr_shapes">
            {" "}
            Difference in improvement distributions shapes
          </Link>
        </li>
        <li>
          <Link to="/statistics">Statistics</Link>{" "}
        </li>
      </ul>
      <Routes>
        <Route path="/averages" element={<Averages />}></Route>
        <Route path="/intial_distr" element={<InitDistr />}></Route>
        <Route path="/diff_impr_shapes" element={<DiffImprShapes />}></Route>
        <Route path="/statistics" element={<Stats />}></Route>
      </Routes>
    </div>
  );
}
